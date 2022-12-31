/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/server/gamecontext.h>
#include "mod.h"

CGameControllerMOD::CGameControllerMOD(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	// Exchange this to a string that identifies your game mode.
	// DM, TDM and CTF are reserved for teeworlds original modes.
	m_pGameType = "Hide";

	m_GameFlags = GAMEFLAG_TEAMS; // GAMEFLAG_TEAMS makes it a two-team gamemode
}

void CGameControllerMOD::Tick()
{
	// this is the main part of the gamemode, this function is run every tick

	IGameController::Tick();
}

void CGameControllerMOD::OnPlayerBeSeeker(int ClientID)
{
	int Hiders=0, Seekers=0;
	for(int i = 0;i < MAX_CLIENTS;i++)
	{
		if(!GameServer()->m_apPlayers[i]) continue;;
		if(GameServer()->m_apPlayers[i]->GetTeam() == TEAM_RED) Seekers++;
		else if(GameServer()->m_apPlayers[i]->GetTeam() == TEAM_BLUE) Hiders++;
	}

	GameServer()->SendChatTarget_Locazition(-1, "'%s' is seeker now!", Server()->ClientName(ClientID));

	if(Hiders > 1)
	{
		GameServer()->SendChatTarget_Locazition(-1, "%s hiders left!", Hiders);
	}else if(Hiders)
	{
		GameServer()->SendChatTarget_Locazition(-1, "Only a hider lefts!", Hiders);
	}

	if(!Hiders && Seekers)
	{
		GameServer()->SendChatTarget_Locazition(-1, "Seekers win!");
		EndRound();
		return;
	}
}