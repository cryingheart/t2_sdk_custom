"""Debug Interface.

This is an interface for debugging. This interface containts operations for:

<ul>
  <li> Force device to halt and stay awake for coredump retrieval </li>
</ul>
"""


from api import base


GROUP_NAME = 'debug'
GROUP_ID = 11


class panic(base.message):
    '''Force device to halt and stay awake for coredump retrieval.'''
    req   = [
    ]
    rsp   = [
        base.status(),
    ]
