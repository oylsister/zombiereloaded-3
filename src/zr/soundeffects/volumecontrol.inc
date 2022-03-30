/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          volumecontrol.inc
 *  Type:          Core
 *  Description:   Basic sound-management API.
 *
 *  Copyright (C) 2009-2013  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

#define ZVOLUME_MAX_TYPE 3

#define ZVOLUME_ZOMBIE 0
#define ZVOLUME_AMBIENT 1
#define ZVOLUME_COUNTDOWN 2

char ZVolumeName[3][] = {"zvolume_zombievolume", "zvolume_ambientvolume", "zvolume_countdownvolume"};
char ZVolumeDesc[3][] = {"Zombie Voice Volume", "Ambient Volume", "Countdown Volume"};

float g_ZVolume[ZVOLUME_MAX_TYPE][MAXPLAYERS+1];

Handle g_hZVolumeCookie[ZVOLUME_MAX_TYPE] = INVALID_HANDLE;

int g_iClientSelected[MAXPLAYERS+1];

void ZVolumeOnPluginStart()
{
    // Command
    RegConsoleCmd("sm_zvolume", ZVolume_MainMenu);
    RegConsoleCmd("sm_zombie_volume", ZVolume_ZombieVoice);
    RegConsoleCmd("sm_ambient_volume", ZVolume_AmbientVolume);
    RegConsoleCmd("sm_countdown_voulme", ZVolume_Countdown);

    // Client Preference Stuff
    for (int i = 0; i < ZVOLUME_MAX_TYPE; i++)
    {
        g_hZVolumeCookie[i] = RegClientCookie(ZVolumeName[i], ZVolumeDesc[i], CookieAccess_Protected);
    }
}

void ZVolumeClientOnCookieCached(int client)
{
    char sValue[8];

    for (int i = 0; i < ZVOLUME_MAX_TYPE; i++)
    {
        GetClientCookie(client, g_hZVolumeCookie[i], sValue, sizeof(sValue));

        if(!sValue[0])
        {
            g_ZVolume[i][client] = 100.0;
            Format(sValue, sizeof(sValue), "%f", g_ZVolume[i][client]);
            SetClientCookie(client, g_hZVolumeCookie[i], sValue);
        }
        else
        {
            g_ZVolume[i][client] = StringToFloat(sValue);
        }
    }
}

void ZVolumeOnClientDisconnect(int client)
{
    for (int i = 0; i < ZVOLUME_MAX_TYPE; i++)
    {
        g_ZVolume[i][client] = 0.0;
    }
}

public Action ZVolume_MainMenu(int client, int args)
{
    Menu menu = new Menu(ZVolumeMainMenu_Handler);

    SetGlobalTransTarget(client);

    char zombievolmenu[128];
    char ambientvolmenu[128];
    char countdownvolmenu[128];

    Format(zombievolmenu, sizeof(zombievolmenu), "%t", "ZVolume Zombie Volume Choice");
    Format(ambientvolmenu, sizeof(ambientvolmenu), "%t", "ZVolmue Ambient Volume Choice");
    Format(countdownvolmenu, sizeof(countdownvolmenu), "%t", "ZVolume Countdown Volume Choice");

    menu.SetTitle("%t", "Zvolume Main Menu");
    menu.AddItem("zombie_volume", zombievolmenu);
    menu.AddItem("ambient_volume", ambientvolmenu);
    menu.AddItem("countdown_volume", countdownvolmenu);

    menu.ExitButton = true;
    menu.Display(client, MENU_TIME_FOREVER);

    return Plugin_Handled;
}

public int ZVolumeMainMenu_Handler(Menu menu, MenuAction action, int client, int param)
{
    switch(action)
    {
        case MenuAction_Select:
        {
            char info[128];
            menu.GetItem(param, info, sizeof(info));

            if (param == 0)
            {
                ZVolume_ZombieVoice(client, 0);
            }
            else if (param == 1)
            {
                ZVolume_AmbientVolume(client, 0);
            }
            else
            {
                ZVolume_Countdown(client, 0);
            }
        }
        case MenuAction_End:
        {
            delete menu;
        }
    }
    return 0;
}

public Action ZVolume_ZombieVoice(int client, int args)
{
    if(args == 0)
    {
        VolumeController(client, ZVOLUME_ZOMBIE);
        return Plugin_Handled;
    }

    char sArgs[64];
    GetCmdArg(1, sArgs, sizeof(sArgs));

    if(StringToInt(sArgs) <= -1 || StringToInt(sArgs) > 100)
    {
        return Plugin_Handled;
    }

    int volume = StringToInt(sArgs);

    SetClientVolume(client, ZVOLUME_ZOMBIE, volume);
    return Plugin_Stop;
}

public Action ZVolume_AmbientVolume(int client, int args)
{
    if(args == 0)
    {
        VolumeController(client, ZVOLUME_AMBIENT);
        return Plugin_Handled;
    }

    char sArgs[64];
    GetCmdArg(1, sArgs, sizeof(sArgs));

    if(StringToInt(sArgs) <= -1 || StringToInt(sArgs) > 100)
    {
        return Plugin_Handled;
    }

    int volume = StringToInt(sArgs);

    SetClientVolume(client, ZVOLUME_AMBIENT, volume);
    return Plugin_Stop;
}

public Action ZVolume_Countdown(int client, int args)
{
    if(args == 0)
    {
        VolumeController(client, ZVOLUME_COUNTDOWN);
        return Plugin_Handled;
    }

    char sArgs[64];
    GetCmdArg(1, sArgs, sizeof(sArgs));

    if(StringToInt(sArgs) <= -1 || StringToInt(sArgs) > 100)
    {
        return Plugin_Handled;
    }

    int volume = StringToInt(sArgs);

    SetClientVolume(client, ZVOLUME_COUNTDOWN, volume);
    return Plugin_Stop;
}

void VolumeController(int client, int type)
{
    Menu menu = new Menu(VolumeControl_Handler);

    SetGlobalTransTarget(client);

    char sTitle[256];

    g_iClientSelected[client] = type;

    switch (type)
    {
        case ZVOLUME_ZOMBIE:
        {
            Format(sTitle, sizeof(sTitle), "%t \n%t", "Zombie Volume Menu Title", "Current Volume", RoundToNearest(g_ZVolume[ZVOLUME_ZOMBIE][client]));
        }
        case ZVOLUME_AMBIENT:
        {
            Format(sTitle, sizeof(sTitle), "%t \n%t", "Ambient Volume Menu Title", "Current Volume", RoundToNearest(g_ZVolume[ZVOLUME_AMBIENT][client]));
        }
        case ZVOLUME_COUNTDOWN:
        {
            Format(sTitle, sizeof(sTitle), "%t \n%t", "Countdown Volume Menu Title", "Current Volume", RoundToNearest(g_ZVolume[ZVOLUME_COUNTDOWN][client]));
        }
    }

    menu.SetTitle(sTitle);
    menu.AddItem("100", "100%");
    menu.AddItem("80", "80%");
    menu.AddItem("60", "60%");
    menu.AddItem("40", "40%");
    menu.AddItem("20", "20%");
    menu.AddItem("0", "0%");

    menu.ExitBackButton = true;
    menu.ExitButton = true;
    menu.Display(client, MENU_TIME_FOREVER);
}

public int VolumeControl_Handler(Menu menu, MenuAction action, int client, int param)
{
    switch (action)
    {
        case MenuAction_Select:
        {
            char info[128];
            menu.GetItem(param, info, sizeof(info));
            int Volume = StringToInt(info);
            SetClientVolume(client, g_iClientSelected[client], Volume);
        }
        case MenuAction_End:
        {
            delete menu;
        }
    }
    return 0;
}

void SetClientVolume(int client, int type, int iVolume)
{
    if(iVolume > 100)
    {
        iVolume = 100;
    }

    else if(iVolume < 0)
    {
        iVolume = 0;
    }

    float fVolume = float(iVolume);

    if(type == ZVOLUME_ZOMBIE)
    {
        g_ZVolume[ZVOLUME_ZOMBIE][client] = fVolume;
        char sValue[64];
        Format(sValue, sizeof(sValue), "%f", g_ZVolume[ZVOLUME_ZOMBIE][client]);
        SetClientCookie(client, g_hZVolumeCookie[ZVOLUME_ZOMBIE], sValue);
        TranslationPrintToChat(client, "Zombie Volume Change", iVolume);
        return;
    }
    else if(type == ZVOLUME_AMBIENT)
    {
        g_ZVolume[ZVOLUME_AMBIENT][client] = fVolume;
        char sValue[64];
        Format(sValue, sizeof(sValue), "%f", g_ZVolume[ZVOLUME_AMBIENT][client]);
        SetClientCookie(client, g_hZVolumeCookie[ZVOLUME_AMBIENT], sValue);
        TranslationPrintToChat(client, "Ambient Volume Change", iVolume);
        return;
    }
    else
    {
        g_ZVolume[ZVOLUME_COUNTDOWN][client] = fVolume;
        char sValue[64];
        Format(sValue, sizeof(sValue), "%f", g_ZVolume[ZVOLUME_COUNTDOWN][client]);
        SetClientCookie(client, g_hZVolumeCookie[ZVOLUME_COUNTDOWN], sValue);
        TranslationPrintToChat(client, "Countdown Volume Change", iVolume);
        return;
    }
}