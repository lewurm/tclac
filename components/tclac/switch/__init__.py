import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_BEEPER, CONF_DISPLAY, ENTITY_CATEGORY_CONFIG

from .. import tclac_ns, TclacClimate

CONF_TCLAC_ID = "tclac_id"

BeeperSwitch = tclac_ns.class_("BeeperSwitch", switch.Switch)
DisplaySwitch = tclac_ns.class_("DisplaySwitch", switch.Switch)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_TCLAC_ID): cv.use_id(TclacClimate),
            cv.Optional(CONF_BEEPER): switch.switch_schema(
                BeeperSwitch,
                entity_category=ENTITY_CATEGORY_CONFIG,
                icon="mdi:volume-high",
                default_restore_mode="DISABLED",
            ),
            cv.Optional(CONF_DISPLAY): switch.switch_schema(
                DisplaySwitch,
                entity_category=ENTITY_CATEGORY_CONFIG,
                icon="mdi:television-ambient-light",
                default_restore_mode="DISABLED",
            ),
        }
    ),
    cv.has_at_least_one_key(CONF_BEEPER, CONF_DISPLAY),
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_TCLAC_ID])

    if CONF_BEEPER in config:
        sw = await switch.new_switch(config[CONF_BEEPER])
        await cg.register_parented(sw, parent)
        cg.add(parent.set_beeper_switch(sw))

    if CONF_DISPLAY in config:
        sw = await switch.new_switch(config[CONF_DISPLAY])
        await cg.register_parented(sw, parent)
        cg.add(parent.set_display_switch(sw))
