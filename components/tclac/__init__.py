import esphome.codegen as cg
from esphome.components import climate, uart

CODEOWNERS = ["@rozenov"]

tclac_ns = cg.esphome_ns.namespace("tclac")
tclacClimate = tclac_ns.class_(
    "tclacClimate", uart.UARTDevice, climate.Climate, cg.PollingComponent
)

AirflowVerticalDirection = tclac_ns.enum("AirflowVerticalDirection", True)
AIRFLOW_VERTICAL_DIRECTION_OPTIONS = {
    "LAST":     AirflowVerticalDirection.LAST,
    "MAX_UP":   AirflowVerticalDirection.MAX_UP,
    "UP":       AirflowVerticalDirection.UP,
    "CENTER":   AirflowVerticalDirection.CENTER,
    "DOWN":     AirflowVerticalDirection.DOWN,
    "MAX_DOWN": AirflowVerticalDirection.MAX_DOWN,
}

AirflowHorizontalDirection = tclac_ns.enum("AirflowHorizontalDirection", True)
AIRFLOW_HORIZONTAL_DIRECTION_OPTIONS = {
    "LAST":      AirflowHorizontalDirection.LAST,
    "MAX_LEFT":  AirflowHorizontalDirection.MAX_LEFT,
    "LEFT":      AirflowHorizontalDirection.LEFT,
    "CENTER":    AirflowHorizontalDirection.CENTER,
    "RIGHT":     AirflowHorizontalDirection.RIGHT,
    "MAX_RIGHT": AirflowHorizontalDirection.MAX_RIGHT,
}
