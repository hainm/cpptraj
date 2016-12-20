with open('configure') as fh, open('configure2', 'w') as fh2:
    content = fh.read().replace('/', '\\\\')
    fh2.write(content)
