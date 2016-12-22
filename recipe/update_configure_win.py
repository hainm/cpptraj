with open('configure') as fh, open('configure2', 'w') as fh2:
    content = fh.read()
    content = content.replace('$VALUE/lib', '$VALUE/bin')
    fh2.write(content)
