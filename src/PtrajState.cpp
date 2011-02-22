#include <cstdio>
#include <cstdlib> // for atoi
#include <cstring>
#include <cctype>
#include "PtrajState.h"
#include "PtrajMpi.h"
#include "CpptrajStdio.h"

// Constructor
PtrajState::PtrajState() { 
  debug=0;
  showProgress=1;
}

// Destructor
PtrajState::~PtrajState() { 
}

/*
 * PtrajState::SetGlobalDebug()
 * Set the debug level for all components of PtrajState.
 */
void PtrajState::SetGlobalDebug(int debugIn) {
  debug = debugIn;
  rprintf("DEBUG LEVEL SET TO %i\n",debug);
  trajFileList.SetDebug(debug);
  refFileList.SetDebug(debug);
  outFileList.SetDebug(debug);
  parmFileList.SetDebug(debug);
  ptrajActionList.SetDebug(debug);
  DFL.SetDebug(debug);
}

/* 
 * PtrajState::ProcessCmdLineArgs()
 * Process arguments on the command line. Process the input file last
 * despite its position on the command line to allow any prmtops to
 * load.
 * Return 1 if unrecognized input on command line.
 * Return 2 if ProcessInputStream indicates we should just quit.
 */
int PtrajState::ProcessCmdLineArgs(int argc, char **argv) {
  int i;
  char *inputFilename;

  inputFilename=NULL;
  for (i=1; i<argc; i++) {
    // --help, -help: Print usage and exit
    if (strcmp(argv[i],"--help")==0 || strcmp(argv[i],"-help")==0) {
      return 1;

    // -p: Topology file
    } else if (strcmp(argv[i],"-p")==0 && i+1!=argc) {
      i++;
      if (debug>0) mprintf("Adding topology file from command line: %s\n", argv[i]);
      parmFileList.Add(argv[i]);

    // -i: Input file
    } else if (strcmp(argv[i],"-i")==0 && i+1!=argc) {
      i++;
      //ProcessInputFile(argv[i]);
      inputFilename=argv[i];

    // -debug: Set overall debug level
    } else if (strcmp(argv[i],"-debug")==0 && i+1!=argc) {
      i++;
      SetGlobalDebug( atoi(argv[i]) );

    // The following 2 are for backwards compatibility with PTRAJ
    // Position 1: TOP file
    } else if (i==1) {
      parmFileList.Add(argv[i]);
    // Position 2: INPUT file
    } else if (i==2) {
      inputFilename=argv[i];

    // Unrecognized
    } else {
      mprintf("PtrajState::ProcessArgs: Unrecognized input on command line: %i: %s\n",
              i,argv[i]);
      return 1;
    }
  }

  if ( ProcessInputStream(inputFilename) ) return 2;

  return 0;
}

/*
 * PtrajState::ProcessInputStream()
 * Process input from the file specified by filename. If filename is NULL
 * process input from STDIN. Set up an argument list and execute commands
 * via the Dispatch routine. 
 * Leading and consectuive whitespace is skipped. \n or NULL executes command.
 * go or EOF ends input read. lines ending with \ continue to the next line.
 */
int PtrajState::ProcessInputStream(char *inputFilename) {
  FILE *infile;
  char ptr,lastchar;
  char inputLine[BUFFER_SIZE]; // Careful, could blow this
  int i;
  bool isStdin;

  isStdin=false;
  // Open input file or STDIN
  if (inputFilename==NULL) {
    // Do not allow read from STDIN when > 1 process
    if (worldsize > 1) {
      mprintf("Error: Reading from STDIN not allowed with more than 1 thread.\n");
      mprintf("       To run cpptraj in parallel please use an input file.\n");
      return 1;
    }
    mprintf("INPUT: Reading Input from STDIN, type \"go\" to run, \"quit\" to exit:\n");
    infile=stdin;
    isStdin=true;
  } else {
    rprintf("INPUT: Reading Input from file %s\n",inputFilename);
    if ( (infile=fopen(inputFilename,"r"))==NULL ) {
      rprintf("Error: Could not open input file %s\n",inputFilename);
      return 1;
    }
  }

  // Read in each line of input. Newline or NULL terminates. \ continues line.
  memset(inputLine,' ',BUFFER_SIZE);
  i=0; // Index in inputLine
  lastchar='0';
  ptr=0;
  if (isStdin) fprintf(stdout,"> ");
  while ( ptr != EOF ) {
    ptr = (char)fgetc(infile);
    //fprintf(stdout,"DEBUG: %i %c %i\n",i,ptr,ptr);
    // newline, NULL, or EOF terminates the line
    if (ptr=='\n' || ptr=='\0' || ptr==EOF) {
      inputLine[i]='\0';
      // If no chars in string continue
      if (strlen(inputLine)==0) continue;
      // If "go" then done reading input
      if (strncmp(inputLine,"go",2)==0) break;
      // If "quit" then abort this - only for stdin
      if (isStdin && strncmp(inputLine,"quit",4)==0) return 1;
      mprintf("  [%s]\n",inputLine);
      // Convert the input line to a list of arguments
      A=new ArgList(inputLine," "); // Space delimited only?
      //A->print();
      // Call Dispatch to perform command in arglist
      Dispatch(); 
      // Reset Input line and Argument List
      delete A;
      memset(inputLine,' ',BUFFER_SIZE);
      i=0;
      if (isStdin) fprintf(stdout,"> ");
      continue;
    }
    // Any consecutive whitespace is skipped
    if (i>0) lastchar=inputLine[i-1];
    if (isspace(ptr) && isspace(lastchar)) continue;
    // Skip leading whitespace
    if (i==0 && isspace(ptr)) {
      while ( (ptr = (char)fgetc(infile))!=EOF )
        if ( !isspace(ptr) ) break;
    } 
    // Forward slash continues to next line. Anything after slash is ignored
    if (ptr=='\\') {
      while ( (ptr = (char)fgetc(infile))!='\n' ) 
        if ( ptr == EOF ) break;
      // NOTE: Insert a space into InputLine?
      continue;
    }
    // Skip any line beginning with # character
    if (i==0 && ptr=='#') {
      while ( (ptr = (char)fgetc(infile))!='\n' ) 
        if ( ptr == EOF ) break;
      if (isStdin) fprintf(stdout,"> ");
      continue;
    }
    inputLine[i++]=ptr;
    // Check to make sure we arent blowing buffer
    if (i==BUFFER_SIZE) {
      rprintf("Error: Input line is greater than BUFFER_SIZE (%u)\n",BUFFER_SIZE);
      if (!isStdin) fclose(infile);
      return 1;
    }
  }

  if (!isStdin) fclose(infile);
  return 0;
}

/* 
 * PtrajState::Dispatch()
 * Send commands to their appropriate classes.
 * The command is tried on each class in turn. If the class rejects command
 * move onto the next one. If command is accepted return.
 * NOTE: Should differentiate between keyword rejection and outright error.
 */
void PtrajState::Dispatch() {
  int err;
  AtomMask *tempMask;  // For ParmInfo
  AmberParm *tempParm; // For ParmInfo
  //printf("    *** %s ***\n",A->ArgLine());
  // First argument is the command
  if (A->Command()==NULL) {
    if (debug>0) mprintf("NULL Command.\n");
    return;
  }

  // Check if command pertains to coordinate lists
  err = trajFileList.Add(A, &parmFileList, worldsize);
  if (err!=CoordFileList::UNKNOWN_COMMAND) return;
  err = refFileList.Add( A, &parmFileList, worldsize);
  if (err!=CoordFileList::UNKNOWN_COMMAND) return;
  err = outFileList.Add( A, &parmFileList, worldsize);
  if (err!=CoordFileList::UNKNOWN_COMMAND) return;

  if (A->CommandIs("parm")) {
    parmFileList.Add(A->getNextString());
    return;
  }

  if (A->CommandIs("noprogress")) {
    showProgress=0;
    mprintf("    noprogress: Progress bar will not be shown.\n");
    return;
  }

  if (A->CommandIs("debug")) {
    SetGlobalDebug( A->getNextInteger(0) );
    return ;
  }

  if (A->CommandIs("parminfo")) {
    if ( (tempParm=parmFileList.GetParm(A->getNextInteger(0)))!=NULL ) {
      tempMask = new AtomMask();
      tempMask->SetMaskString( A->getNextMask() );
      tempMask->SetupCharMask( tempParm, debug);
      for (int atom=0; atom < tempParm->natom; atom++) {
        if (tempMask->AtomInCharMask(atom)) tempParm->AtomInfo(atom);
      }
      delete tempMask;
    }
    return;
  }

  // Check if command pertains to an action
  if ( ptrajActionList.Add(A)==0 ) return; 

  mprintf("Warning: Unknown Command %s.\n",A->Command());
}

/* 
 * PtrajState::Run()
 * Process trajectories in trajFileList. Each frame in trajFileList is sent
 * to the actions in ptrajActionList for processing.
 */
int PtrajState::Run() {
  int maxFrames;
  int global_set;  // Incremented for every frame read
  int outputSet;   // Output file frame, changes based on active parm file
  int actionSet;   // Internal data frame
  int process_set; // Incremented for every frame processed
  int lastPindex;  // Index of the last loaded parm file
  TrajFile *T;
  std::list<TrajFile*>::iterator it;
  AmberParm *CurrentParm;
  Frame *CurrentFrame;
  FrameList refFrames;

  // ========== S E T U P   P H A S E ========== 
  // Calculate frame division among trajectories
  maxFrames=trajFileList.SetupFrames();

  // Parameter file information
  parmFileList.Print();

  // Setup reference frames if reference files were specified 
  refFileList.SetupRefFrames(&refFrames);
  refFrames.Info();

  // Output traj
  mprintf("\nOUTPUT TRAJECTORIES:\n");
  outFileList.Info();
 
  // Set max frames in the data set list
  DSL.SetMax(maxFrames); 
  
  // Initialize actions and set up data set and data file list
  ptrajActionList.Init( &DSL, &refFrames, &DFL);

  // ========== R U N  P H A S E ==========
  // Loop over every trajectory in trajFileList
  actionSet=0;
  outputSet=0;
  global_set=0;
  process_set=0;
  lastPindex=-1;
  rprintf("BEGIN TRAJECTORY PROCESSING:\n");
  for (it=trajFileList.begin(); it!=trajFileList.end(); it++) {
    T=(*it);
    // Open up the trajectory file. Return val >0 indicates traj should be skipped 
    if ( T->Begin(&actionSet, showProgress) ) {
      T->End();
      continue;
    }

    // Calculate output Start for trajectory parm
    outputSet=(worldrank * (T->total_read_frames / worldsize)) + T->P->outFrame;
#ifdef DEBUG
    dbgprintf("\tOutputstart for %s is %i\n", T->P->parmName, outputSet);
#endif

    // Debug info
    if (debug>0)
      rprintf("    Global set=%i, action set=%i, output set=%i, process set=%i\n",
              global_set, actionSet, outputSet, process_set);

    // Set Current Parm if different from last parm
    if (lastPindex != T->P->pindex) {
      CurrentParm = T->P;

      // If Parm has changed set up the Action list for new topology file
      if (ptrajActionList.Setup( &CurrentParm )) {
        mprintf("WARNING: Could not set up actions for %s: skipping.\n",
                CurrentParm->parmName);
        continue;
      }
      //fprintf(stdout,"DEBUG: After setup of Actions in PtrajState parm name is %s\n",
      //        CurrentParm->parmName);
    }

    // Loop over every Frame in trajectory
    while ( T->NextFrame(&global_set) ) {
      // Set current Frame
      CurrentFrame = T->F;
      //if (showProgress) T->progressBar();
      // Perform Actions on Frame
      ptrajActionList.DoActions(&CurrentFrame, actionSet);
      // Do Output
      outFileList.Write(outputSet, CurrentFrame, CurrentParm);
#ifdef DEBUG
      dbgprintf("\tDEBUG: %30s: %4i\n",CurrentParm->parmName,CurrentParm->outFrame);
#endif
      // Increment output, action, and processed frame counters
      outputSet++;
      actionSet++; 
      process_set++;
    }

    // Close the trajectory file
    T->End();
    // Update how many frames across all threads have been written for parm
    T->P->outFrame+=T->total_read_frames;
    mprintf("\n");
    lastPindex = T->P->pindex;
  }

  rprintf("Read %i frames and processed %i frames.\n",global_set,process_set);
  rprintf("Final output set: %i\n",outputSet);

  // Close output traj
  outFileList.Close();

  // Do action output
  ptrajActionList.Print( );

  // Print Dataset information
  DSL.Info();
  // Print Datafile information
  DFL.Info();

  // Do dataset output - first sync datasets
  DSL.Sync();
  // Only Master does DataFile output
  if (worldrank==0)
    DFL.Write();
 
  return 0;
}
