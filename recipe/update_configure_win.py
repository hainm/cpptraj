with open('configure') as fh, open('configure2', 'w') as fh2:
    content = fh.read().replace('/', '\\\\')
    content = content.replace('\\\\dev\\\\null', '/dev/null')
    content = content.replace('\\\\dev\\\\stderr', '/dev/stderr')
    content = content.replace('\\\\bin\\\\rm', '/bin/rm')
    fh2.write(content)
