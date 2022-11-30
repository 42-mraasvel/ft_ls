import itertools

OPTIONS = [ 'l', 'r', 'R', 't', 'a' ]
result = []
for i in range(1, 5+1):
	for x in itertools.combinations(OPTIONS, i):
		result.append(x)
result = [ '"-' + ''.join(x) + '"' for x in result ]
print('OPTIONS=(' + ' '.join(result) + ')')
