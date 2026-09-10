import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import ENTITY_CATEGORY_CONFIG

from .. import tclac_ns, tclacClimate

CODEOWNERS = ["@junkfix"]

CONF_TCLAC_ID = "tclac_id"
CONF_VERTICAL_LOUVER = "vertical_louver"
CONF_HORIZONTAL_LOUVER = "horizontal_louver"

VerticalLouverSelect = tclac_ns.class_("VerticalLouverSelect", select.Select)
HorizontalLouverSelect = tclac_ns.class_("HorizontalLouverSelect", select.Select)

# First option hands the axis back to climate swing_mode; the rest park the
# louver at a fixed position. Order and spelling must match the option<->enum
# tables in airflow_select.cpp.
VERTICAL_LOUVER_OPTIONS = ["Swing", "Max up", "Up", "Center", "Down", "Max down"]
HORIZONTAL_LOUVER_OPTIONS = ["Swing", "Max left", "Left", "Center", "Right", "Max right"]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_TCLAC_ID): cv.use_id(tclacClimate),
        cv.Optional(CONF_VERTICAL_LOUVER): select.select_schema(
            VerticalLouverSelect,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:arrow-up-down",
        ),
        cv.Optional(CONF_HORIZONTAL_LOUVER): select.select_schema(
            HorizontalLouverSelect,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:arrow-left-right",
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_TCLAC_ID])

    if CONF_VERTICAL_LOUVER in config:
        sel = await select.new_select(
            config[CONF_VERTICAL_LOUVER], options=VERTICAL_LOUVER_OPTIONS
        )
        await cg.register_parented(sel, parent)
        cg.add(parent.set_vertical_louver_select(sel))

    if CONF_HORIZONTAL_LOUVER in config:
        sel = await select.new_select(
            config[CONF_HORIZONTAL_LOUVER], options=HORIZONTAL_LOUVER_OPTIONS
        )
        await cg.register_parented(sel, parent)
        cg.add(parent.set_horizontal_louver_select(sel))
