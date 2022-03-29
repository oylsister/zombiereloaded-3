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

#include <clientprefs>

#define ZVOLUME_MAX_TYPE 3

#define ZVOLUME_ZOMBIE 0
#define ZVOLUME_AMBIENT 1
#define ZVOLUME_COUNTDOWN 2

char ZVolumeName[3][] = {"zvolume_zombievolume", "zvolume_ambientvolume", "zvolume_countdownvolume"};
char ZVolumeDesc[3][] = {"Zombie Voice Volume", "Ambient Volume", "Countdown Volume"};

float g_ZVolume[ZVOLUME_MAX_TYPE][MAXPLAYERS+1];

Handle g_hZVolumeCookie[ZVOLUME_MAX_TYPE] = INVALID_HANDLE;

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
            g_ZVolume[i][client] = 1.0;
            SetClientCookie(client, g_hZVolumeCookie, 1.0);
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

void ZVolumeSetVolume(int client, int type, float value)
{
    g_ZVolume[type][client] = value;
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
    Format(countdownvolmenu, sizeof(countdownvolmenu), "%t", "ZVolume Countdown Volume Choice")

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
    return 0;
}