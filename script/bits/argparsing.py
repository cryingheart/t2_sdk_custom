"""
Update config from argparse when using CLI.

"""


import sys
import logging
import argparse


logger = logging.getLogger(__name__)


def default_parser():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument('--verbose', '-v', action='count', default=0)
    return parser
