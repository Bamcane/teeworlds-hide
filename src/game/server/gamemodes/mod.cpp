/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/server/gamecontext.h>
#include <engine/shared/config.h>
#include "mod.h"

CGameControllerMOD::CGameControllerMOD(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	// Exchange this to a string that identifies your game mode.
	// DM, TDM and CTF are reserved for teeworlds original modes.
	m_pGameType = "Hide";

	m_LastPlayersNum = 0;

	//m_GameFlags = GAMEFLAG_TEAMS; // GAMEFLAG_TEAMS makes it a two-team gamemode
}

void CGameControllerMOD::Tick()
{
	// this is the main part of the gamemode, this function is run every tick
	int Hiders=0, Seekers=0;
	for(int i = 0;i < MAX_CLIENTS;i++)
	{
		if(!Server()->ClientIngame(i) || !GameServer()->m_apPlayers[i]) continue;
		if(GameServer()->m_apPlayers[i]->GetTeam() == TEAM_RED) Seekers++;
		else if(GameServer()->m_apPlayers[i]->GetTeam() == TEAM_BLUE) Hiders++;
	}

	if(m_LastPlayersNum < 2 && (Hiders + Seekers) >= 2)
	{
		GameServer()->SendBroadcast("", -1);
		StartRound();
	}else if(m_LastPlayersNum < 2 && (Server()->Tick()%25) == 0) GameServer()->SendBroadcast_VL("Wait game start!", -1);

	if((Server()->Tick()-m_RoundStartTick) >= g_Config.m_SvTimelimit*Server()->TickSpeed()*60 - Server()->TickSpeed()*30)
	{
		if(!m_HiderAttackTime)
		{
			GameServer()->SendChatTarget_Locazition(-1, "Hider attack time!!");
			GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE, -1);
		}
		m_HiderAttackTime = 1;
	}else m_HiderAttackTime = 0;

	if(!IsGameOver())
	{
		if(!Hiders && Seekers > 0)
		{
			GameServer()->SendChatTarget_Locazition(-1, "Seekers win!");
			GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE, -1);
			EndRound();
			return;
		}
	}
	IGameController::Tick();

	m_LastPlayersNum = Hiders + Seekers;
}

void CGameControllerMOD::OnPlayerBeSeeker(int ClientID)
{
	GameServer()->SendChatTarget_Locazition(-1, "'%s' is seeker now!", Server()->ClientName(ClientID));	
	GameServer()->m_apPlayers[ClientID]->SetTeam(TEAM_RED, false);
	// this is the main part of the gamemode, this function is run every tick
	int Hiders=0, Seekers=0;
	for(int i = 0;i < MAX_CLIENTS;i++)
	{
		if(!Server()->ClientIngame(i) || !GameServer()->m_apPlayers[i]) continue;
		if(GameServer()->m_apPlayers[i]->GetTeam() == TEAM_RED) Seekers++;
		else if(GameServer()->m_apPlayers[i]->GetTeam() == TEAM_BLUE) Hiders++;
	}

	if(Hiders > 1)
	{
		GameServer()->SendChatTarget_Locazition(-1, "%d hiders left!", Hiders);
	}else if(Hiders)
	{
		GameServer()->SendChatTarget_Locazition(-1, "Only a hider lefts!");
	}
}

int CGameControllerMOD::GetAutoTeam(int NotThisID)
{
	if(IsGameOver() || m_Warmup)
		return TEAM_BLUE;
	else return TEAM_RED;
}