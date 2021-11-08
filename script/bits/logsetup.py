import logging


logger = logging.getLogger(__name__)


def config_logger(level=0):
    """
    level=1  # CRITICAL
    ...
    level=5  # DEBUG
    """
    levels = (50, 40, 30, 20, 10, 0)
    level = levels[min(level, len(levels) - 1)]
    logging.basicConfig(
        level=level,
        format='%(asctime)-15s '
        '%(relativeCreated)6d '
        '%(threadName)-15s '
        '%(message)s',
    )
    if level < 50:
        print(f'log level changed: {logger.level} -> {level}')
