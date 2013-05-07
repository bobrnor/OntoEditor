import sys
import getopt

if __name__ == '__main__':
	opts, extraparams = getopt.getopt(sys.argv[1:], '', '')
	source_path = extraparams[0]
	