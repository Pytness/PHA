from dll import PHA256
import string

INITIAL_STATES = 'e6454cd78e6ce6776280b3470aa84ce7debc19b786e3b356031f19c6d732e696'

def diff(hash_a, hash_b):
	result = str.join('', [
		' ' if ca != cb else '·'
		for ca, cb in zip(hash_a, hash_b)
	])

	return result


def generate_cases(test_length: int):
	NULL_BYTES =  b'\0' * (test_length - 1)

	return [
		NULL_BYTES + c.encode('ascii')
		for c in string.ascii_letters
	]

if __name__ == '__main__':
	pha = PHA256()


	longest_nums = 0

	# last = INITIAL_STATES
	# tests = generate_cases(64)

	# max_zero_count = 0

	# for i in range(1, 128):
	# 	tests = generate_cases(i)
	# 	test_iter = iter(tests)

	# 	last = pha.hexdigest(next(test_iter))

	# 	for test in test_iter:
	# 		h = pha.hexdigest(test)
	# 		d = diff(last, h)
	# 		# print('%s -> %s' % (h, d))
	# 		zero_count = d.count('0')

	# 		if zero_count > max_zero_count:
	# 			max_zero_count = zero_count
	# 			print(i, zero_count)

	# 		last = h


	tests = generate_cases(38)
	last = INITIAL_STATES

	print(INITIAL_STATES)

	for test in tests:
		h = pha.hexdigest(test)
		d = diff(last, h)
		print('%s -> %s' % (h, d))
		last = h


