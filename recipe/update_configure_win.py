with open('configure') as fh, open('configure2', 'w') as fh2:
    place_holder = 'placeholder'
    content = fh.read()
    content = content.replace('CPPTRAJLIB=$VALUE/lib', place_holder)
    content = content.replace('$VALUE/lib', '$VALUE/bin')
    content = content.replace('-lopenblas', '-lopenblaspy')
    content = content.replace('CPPTRAJLIB=$VALUE/lib', place_holder)
    content = content.replace(place_holder, 'CPPTRAJLIB=$VALUE/lib')
    fh2.write(content)
