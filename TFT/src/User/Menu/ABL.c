#include "ABL.h"
#include "includes.h"

static uint8_t ublSlot;
static bool ublIsSaving = true;
static bool ublSlotSaved = false;

// called by parseAck() to notify ABL process status
void ablUpdateStatus(bool succeeded)
{
  bool savingEnabled = true;

  init_label(tempTitle);
  tempTitle.index = LABEL_ABL_SETTINGS;
  LABEL_CHAR(tempMsg, LABEL_BL_COMPLETE);

  switch (infoMachineSettings.leveling)
  {
    case BL_BBL:
      tempTitle.index = LABEL_ABL_SETTINGS_BBL;
      break;

    case BL_UBL:
      savingEnabled = false;

      tempTitle.index = LABEL_ABL_SETTINGS_UBL;
      sprintf(strchr(tempMsg, '\0'), "\n %s", textSelect(LABEL_BL_SMART_FILL));
      break;

    default:
      break;
  }

  if (succeeded)  // if bed leveling process successfully terminated, allow to save to EEPROM
  {
    BUZZER_PLAY(SOUND_SUCCESS);

    if (savingEnabled && infoMachineSettings.EEPROM == 1)
    {
      sprintf(strchr(tempMsg, '\0'), "\n %s", textSelect(LABEL_EEPROM_SAVE_INFO));

      popupDialog(DIALOG_TYPE_SUCCESS, tempTitle.index, tempMsg, LABEL_CONFIRM, LABEL_CANCEL, saveEepromSettings, NULL, NULL);
    }
    else
    {
      popupReminder(DIALOG_TYPE_SUCCESS, tempTitle.index, tempMsg);
    }
  }
  else  // if bed leveling process failed, provide an error dialog
  {
    BUZZER_PLAY(SOUND_ERROR);

    popupReminder(DIALOG_TYPE_ERROR, tempTitle.index, LABEL_PROCESS_ABORTED);
  }
}

// start ABL process
void ablStart(void)
{
  storeCmd("G28\n");

  switch (infoMachineSettings.leveling)
  {
    case BL_BBL:  // if Bilinear Bed Leveling
      storeCmd("G29\n");
      break;

    case BL_UBL:  // if Unified Bed Leveling
      storeCmd("G29 P1\n");
      storeCmd("G29 P3\n");  // run this multiple times since it only fills some missing points, not all
      storeCmd("G29 P3\n");
      storeCmd("G29 P3\n");
      // find Mean Mesh Height: with option "C" this will automatically execute a G29 P6 C[mean height].
      // Ideally the Mesh is adjusted for a Mean Height of 0.00 and the Z-Probe measuring 0.0 at the Z homing position
      storeCmd("G29 P5 C\n");
      break;

    default:  // if any other Auto Bed Leveling
      storeCmd("G29\n");
      break;
  }

  if (infoMachineSettings.firmwareType != FW_REPRAPFW)
    storeCmd("M118 P0 ABL Completed\n");
}

static void ublSaveloadConfirm(void)
{
  if (!ublIsSaving)
    storeCmd("G29 L%d\n", ublSlot);
  else
    ublSlotSaved = storeCmd("G29 S%d\n", ublSlot);
}

static void menuUBLSaveLoad(void)
{
  MENUITEMS UBLSaveLoadItems = {
    // title
    LABEL_ABL_SETTINGS_UBL_SAVE,
    // icon                          label
    {
      {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT0},
      {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT1},
      {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT2},
      {ICON_EEPROM_SAVE,             LABEL_ABL_SLOT3},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_NULL,                    LABEL_NULL},
      {ICON_BACK,                    LABEL_BACK},
    }
  };

  KEY_VALUES key_num = KEY_IDLE;

  if (!ublIsSaving)
  {
    UBLSaveLoadItems.title.index = LABEL_ABL_SETTINGS_UBL_LOAD;

    for (int i = 0; i < 4; i++)
    {
      UBLSaveLoadItems.items[i].icon = ICON_EEPROM_RESTORE;
    }
  }

  menuDrawPage(&UBLSaveLoadItems);

  while (MENU_IS(menuUBLSaveLoad))
  {
    key_num = menuKeyGetValue();

    switch (key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
      case KEY_ICON_3:
        ublSlot = key_num;

        popupDialog(DIALOG_TYPE_QUESTION, UBLSaveLoadItems.title.index, LABEL_CONFIRMATION, LABEL_CONFIRM, LABEL_CANCEL,
                    ublSaveloadConfirm, NULL, NULL);
        break;

      case KEY_ICON_7:
        if (ublSlotSaved == true && infoMachineSettings.EEPROM == 1)
          popupDialog(DIALOG_TYPE_QUESTION, LABEL_ABL_SETTINGS_UBL, LABEL_ABL_SLOT_EEPROM, LABEL_CONFIRM, LABEL_CANCEL,
                      saveEepromSettings, NULL, NULL);

        ublSlotSaved = false;

        CLOSE_MENU();
        break;

      default:
        break;
    }

    loopProcess();
  }
}

void menuUBLSave(void)
{
  ublIsSaving = true;

  OPEN_MENU(menuUBLSaveLoad);
}

void menuUBLLoad(void)
{
  ublIsSaving = false;

  OPEN_MENU(menuUBLSaveLoad);
}
