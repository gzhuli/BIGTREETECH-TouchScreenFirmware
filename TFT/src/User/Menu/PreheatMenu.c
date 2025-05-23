#include "PreheatMenu.h"
#include "includes.h"

typedef enum
{
  BOTH = 0,
  BED_PREHEAT = 1,
  NOZZLE0_PREHEAT = 2,
  PREHEAT_TOOL_COUNT = 3,
} TOOLPREHEAT;

static const GUI_POINT preheat_title = {ICON_WIDTH / 2, PREHEAT_TITLE_Y };
static const GUI_POINT preheat_val_tool = {ICON_WIDTH - BYTE_WIDTH / 2, PREHEAT_TOOL_Y};
static const GUI_POINT preheat_val_bed = {ICON_WIDTH - BYTE_WIDTH / 2, PREHEAT_BED_Y};

static void setPreheatIcon(ITEM * item, TOOLPREHEAT nowHeater)
{
  switch (nowHeater)
  {
    case BOTH:
      item->icon = ICON_PREHEAT_BOTH;
      item->label.index = LABEL_PREHEAT_BOTH;
      break;

    case BED_PREHEAT:
      item->icon = ICON_BED;
      item->label.index = LABEL_BED;
      break;

    case NOZZLE0_PREHEAT:
      item->icon = ICON_NOZZLE;
      item->label.index = LABEL_NOZZLE;
      break;

    default:
      break;
  }
}

// redraw Preheat icon details
void refreshPreheatIcon(PREHEAT_STORE * preheatStore, uint8_t index, bool redrawIcon)
{
  LIVE_INFO lvIcon;

  lvIcon.iconIndex = ICON_PREHEAT;
  lvIcon.enabled[0] = true;
  lvIcon.enabled[1] = true;
  lvIcon.enabled[2] = true;

  // set preheat title properties
  lvIcon.lines[0].h_align = CENTER;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].fn_color = PH_VAL_COLOR;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = preheat_title;
  lvIcon.lines[0].font = FONT_SIZE_NORMAL;

  // set preheat tool propertites
  lvIcon.lines[1].h_align = RIGHT;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].fn_color = PH_VAL_COLOR;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = preheat_val_tool;
  lvIcon.lines[1].font = FONT_SIZE_NORMAL;

  // set preheat bed properties
  lvIcon.lines[2].h_align = RIGHT;
  lvIcon.lines[2].v_align = CENTER;
  lvIcon.lines[2].fn_color = PH_VAL_COLOR;
  lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[2].pos = preheat_val_bed;
  lvIcon.lines[2].font = FONT_SIZE_NORMAL;

  lvIcon.lines[0].text = preheatStore->preheat_name[index];

  char temptool[5];
  char tempbed[5];

  sprintf(temptool, "%d", preheatStore->preheat_hotend[index]);
  sprintf(tempbed, "%d", preheatStore->preheat_bed[index]);

  lvIcon.lines[1].text = temptool;
  lvIcon.lines[2].text = tempbed;

  showLiveInfo(index, &lvIcon, redrawIcon);
}

void menuPreheat(void)
{
  MENUITEMS preheatItems = {
    // title
    LABEL_PREHEAT,
    // icon                          label
    {
      {ICON_PREHEAT,                 LABEL_NULL},
      {ICON_PREHEAT,                 LABEL_NULL},
      {ICON_PREHEAT,                 LABEL_NULL},
      {ICON_PREHEAT,                 LABEL_NULL},
      {ICON_PREHEAT,                 LABEL_NULL},
      {ICON_PREHEAT,                 LABEL_NULL},
      {ICON_PREHEAT_BOTH,            LABEL_PREHEAT_BOTH},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  static TOOLPREHEAT nowHeater = BOTH;

  KEY_VALUES key_num = KEY_IDLE;
  PREHEAT_STORE preheatStore;

  setPreheatIcon(&preheatItems.items[KEY_ICON_6], nowHeater);

  menuDrawPage(&preheatItems);

  W25Qxx_ReadBuffer((uint8_t *) &preheatStore, PREHEAT_STORE_ADDR, sizeof(PREHEAT_STORE));

  for (int i = 0; i < PREHEAT_COUNT; i++)
  {
    refreshPreheatIcon(&preheatStore, i, false);
  }

  while (MENU_IS(menuPreheat))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
      case KEY_ICON_3:
      case KEY_ICON_4:
      case KEY_ICON_5:
        switch (nowHeater)
        {
          case BOTH:
            heatSetTargetTemp(BED, preheatStore.preheat_bed[key_num], FROM_GUI);
            heatSetTargetTemp(heatGetCurrentHotend(), preheatStore.preheat_hotend[key_num], FROM_GUI);
            break;

          case BED_PREHEAT:
            heatSetTargetTemp(BED, preheatStore.preheat_bed[key_num], FROM_GUI);
            break;

          case NOZZLE0_PREHEAT:
            heatSetTargetTemp(heatGetCurrentHotend(), preheatStore.preheat_hotend[key_num], FROM_GUI);
            break;

          default:
            break;
        }

        refreshPreheatIcon(&preheatStore, key_num, false);
        break;

      case KEY_ICON_6:
        nowHeater = (TOOLPREHEAT)((nowHeater + 1) % PREHEAT_TOOL_COUNT);
        setPreheatIcon(&preheatItems.items[key_num], nowHeater);

        menuDrawItem(&preheatItems.items[key_num], key_num);
        break;

      case KEY_ICON_7:
        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}
