#ifndef INC_PARALLEL_H
#define INC_PARALLEL_H
/** In C++, MPI is a reserved namespace, but since Amber uses it to define
  * parallel build we need to work with it. Change MPI to CPPTRAJ_MPI for
  * this header file only.
  */
#ifdef MPI
# undef MPI
# define CPPTRAJ_MPI
# include <mpi.h>
#endif
/// Static class, Cpptraj C++ interface to C MPI routines.
/** NOTE: I have decided to use a class instead of a namespace to have the
  *       option of making things private. Not sure if that is really
  *       necessary.
  */
class Parallel {
  public:
    /// C++ class wrapper around MPI comm routines.
    class Comm;
    /// \return MPI_WORLD_COMM Comm
    static Comm const& World() { return world_; }
    /// Initialize parallel environment
    static int Init(int, char**);
    /// Stop parallel environment
    static int End();
  private:
#   ifdef CPPTRAJ_MPI
    static void printMPIerr(int, const char*, int);
    static int checkMPIerr(int, const char*, int);
    static int Abort(int);
#   endif
    static Comm world_;
};

class Parallel::Comm {
  public:
    Comm() : rank_(0), size_(1) {}
    int Rank()    const { return rank_;      }
    int Size()    const { return size_;      }
    bool Master() const { return rank_ == 0; }
#   ifdef CPPTRAJ_MPI
    Comm(MPI_Comm);
    void Barrier() const;
    int Reduce(void*, void*, int, MPI_Datatype, MPI_Op) const;
    int SendMaster(void*, int, int, MPI_Datatype) const;
    int AllReduce(void*, void*, int, MPI_Datatype, MPI_Op) const;
#   endif
  private:
#   ifdef CPPTRAJ_MPI
    MPI_Comm comm_;
#   endif
    int rank_;
    int size_;
};
#ifdef CPPTRAJ_MPI
# undef CPPTRAJ_MPI
# define MPI
#endif
#endif
