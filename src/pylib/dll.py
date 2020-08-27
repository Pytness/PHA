#!/usr/bin/env python3

'''Provides PHA256'''

import ctypes
import pathlib

# Load the shared library into ctypes
LIBNAME = pathlib.Path().absolute() / "libpha.so"
PHALIB = ctypes.CDLL(LIBNAME)

PHALIB.PHA256_new.argtypes = []
PHALIB.PHA256_new.restype = ctypes.c_void_p

PHALIB.digest.argtypes = [
	ctypes.c_void_p,
	ctypes.POINTER(ctypes.c_ubyte),
	ctypes.POINTER(ctypes.c_ubyte),
	ctypes.c_uint64,
]
PHALIB.digest.restype = None

class PHA256():
	'''A PHA-256 hash object'''
	def __init__(self):
		self.__pha_instance__ = ctypes.c_void_p(PHALIB.PHA256_new())

	def digest(self, data):
		'''Return the digest of the bytes passed as a bytes object.'''

		if not isinstance(data, bytes):
			raise ValueError('Data must be bytes')

		raw_digest_buffer = ctypes.cast(
			ctypes.create_string_buffer(32),
			ctypes.POINTER(ctypes.c_ubyte)
		)

		raw_data = ctypes.cast(
			ctypes.create_string_buffer(data),
			ctypes.POINTER(ctypes.c_ubyte)
		)

		data_length = ctypes.c_uint64(len(data))

		PHALIB.digest(self.__pha_instance__, raw_digest_buffer, raw_data, data_length)

		del data
		del raw_data
		del data_length

		# for i in range(32):
		digest_bytes = bytearray(raw_digest_buffer[:32])
		del raw_digest_buffer

		return digest_bytes

	def hexdigest(self, data):
		'''Return the digest of the bytes passed as the hexadecimal representation of the bytes object.'''
		return self.digest(data).hex()
