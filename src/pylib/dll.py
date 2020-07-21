#!/usr/bin/env python3

import ctypes
import pathlib
import numpy as np
import hashlib
import os
from timeit import default_timer as timer
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.colors import LightSource

# Load the shared library into ctypes
libname = pathlib.Path().absolute() / "libpha.so"
phalib = ctypes.CDLL(libname)

phalib.PHA256_new.argtypes = []
phalib.PHA256_new.restype = ctypes.c_void_p

phalib.digest.argtypes = [
	ctypes.c_void_p,
	ctypes.POINTER(ctypes.c_ubyte),
	ctypes.POINTER(ctypes.c_ubyte),
	ctypes.c_uint64,
]
phalib.digest.restype = None

class PHA256(object):
	def __init__(self):
		self.__pha_instance__ = ctypes.c_void_p(phalib.PHA256_new())
		# print(f'pha instance: {self.__pha_instance__}')

	def digest(self, data):
		if not type(data) is bytes:
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

		phalib.digest(self.__pha_instance__, raw_digest_buffer, raw_data, data_length)

		del data
		del raw_data
		del data_length

		# for i in range(32):
		digest_bytes = bytearray(raw_digest_buffer[:32])
		del raw_digest_buffer

		return digest_bytes

	def hexdigest(self, data):
		return self.digest(data).hex()
