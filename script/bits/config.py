"""
Default config module

Shared configuration for this library.
This is not argparse dependent.

"""


from dataclasses import dataclass


@dataclass
class GpioConfig:
    wakeup_pin: int = 0
    wakeup_level: int = 0
    irq_pin: int = 0
    irq_mode: int = 0
    host_wakeup_pin: int = 0
    host_irq_pin: int = 0


@dataclass
class RpiGpioConfig(GpioConfig):
    wakeup_pin: int = 3
    wakeup_level: int = 0
    irq_pin: int = 4
    irq_mode: int = 2
    host_wakeup_pin: int = 20
    host_irq_pin: int = 21
