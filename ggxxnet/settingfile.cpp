/* for Visual Studio 8.0 */
#ifdef _MSC_VER
#if (_MSC_VER >= 1400)
#define POINTER_64 __ptr64
#endif
#endif

//******************************************************************
// include
//******************************************************************
#include "ggxxnet.h"
#include "netMgr.h"
#include "settingfile.h"
#include "zlib.h"
#include "util.h"
#include "md5.h"

#include <locale.h>
#include <mbstring.h>
#include <string>
#include <vector>
#include "internet.h"
#include <sstream>
#include <fstream>
//******************************************************************
// function
//******************************************************************



std::vector<std::string> split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		if (token.compare("null") == 0) {
			res.push_back("");
		}
		else {
			res.push_back(token);
		}
	}

	res.push_back(s.substr(pos_start));
	res.pop_back();
	return res;
}

void getSettings(void) {
	char response[1024];
	char command[256];
	std::ifstream configFile("config.txt");
	std::vector<std::string> config;
	std::string line;
	while (std::getline(configFile, line))
	{
		config.push_back(line);
	}
	configFile.close();
	std::string playerId = config.back();
	config.pop_back();
	std::string path = config.back();
	config.pop_back();
	std::string server = config.back();
	config.pop_back();
	sprintf(command, "{\"playerId\": \"%s\"}", playerId.c_str());
	std::string address = path + "/get-config";
	makePost(command, strlen(command), 1024, server, address, response);
	std::string res = response;
	std::vector<std::string> splitted = split(res, "|");
	//////////////////////////////////////////////////////////////////////
	//watchMaxNodes
	std::string watchMaxNodes = splitted.back();
	g_setting.watchMaxNodes = atoi(watchMaxNodes.c_str());
	splitted.pop_back();
	//watchSaveReplay
	std::string watchSaveReplay = splitted.back();
	g_setting.watchSaveReplay = atoi(watchSaveReplay.c_str());
	splitted.pop_back();
	//watchIntrusion
	std::string watchIntrusion = splitted.back();
	g_setting.watchIntrusion = atoi(watchIntrusion.c_str());
	splitted.pop_back();
	//watchBroadcast
	std::string watchBroadcast = splitted.back();
	g_setting.watchBroadcast = atoi(watchBroadcast.c_str());
	splitted.pop_back();
	//msg
	std::string msg = splitted.back();
	strcpy(g_setting.msg, msg.c_str());
	splitted.pop_back();
	//rounds
	std::string rounds = splitted.back();
	g_setting.rounds = atoi(rounds.c_str());
	splitted.pop_back();
	//slowRate
	std::string slowRate = splitted.back();
	g_setting.slowRate = atoi(slowRate.c_str());
	splitted.pop_back();
	//totalError
	std::string totalError = splitted.back();
	g_setting.totalError = atoi(totalError.c_str());
	splitted.pop_back();
	//totalDraw
	std::string totalDraw = splitted.back();
	g_setting.totalDraw = atoi(totalDraw.c_str());
	splitted.pop_back();
	//totalLose
	std::string totalLose = splitted.back();
	g_setting.totalLose = atoi(totalLose.c_str());
	splitted.pop_back();
	//totalWin
	std::string totalWin = splitted.back();
	g_setting.totalWin = atoi(totalWin.c_str());
	splitted.pop_back();
	//totalBattle
	std::string totalBattle = splitted.back();
	g_setting.totalBattle = atoi(totalBattle.c_str());
	splitted.pop_back();
	//score
	std::string score = splitted.back();
	g_setting.score = atoi(score.c_str());
	splitted.pop_back();
	//rank
	std::string rank = splitted.back();
	g_setting.rank = atoi(rank.c_str());
	splitted.pop_back();
	//wins
	std::string wins = splitted.back();
	g_setting.wins = atoi(wins.c_str());
	splitted.pop_back();
	//showfps
	std::string showfps = splitted.back();
	g_setting.showfps = atoi(showfps.c_str());
	splitted.pop_back();
	//dispInvCombo
	std::string dispInvCombo = splitted.back();
	g_setting.dispInvCombo = atoi(dispInvCombo.c_str());
	splitted.pop_back();
	//useEx
	std::string useEx = splitted.back();
	g_setting.useEx = atoi(useEx.c_str());
	splitted.pop_back();
	//wait
	std::string wait = splitted.back();
	g_setting.wait = atoi(wait.c_str());
	splitted.pop_back();
	//ignoreSlow
	std::string ignoreSlow = splitted.back();
	g_setting.ignoreSlow = atoi(ignoreSlow.c_str());
	splitted.pop_back();
	//ignoreMisNode
	std::string ignoreMisNode = splitted.back();
	g_setting.ignoreMisNode = atoi(ignoreMisNode.c_str());
	splitted.pop_back();
	//delay
	std::string delay = splitted.back();
	g_setting.delay = atoi(delay.c_str());
	splitted.pop_back();
	//enableNet
	std::string enableNet = splitted.back();
	g_setting.enableNet = atoi(enableNet.c_str());
	splitted.pop_back();
	//trip
	std::string trip = splitted.back();
	strcpy(g_setting.trip, "");
	splitted.pop_back();
	//userName
	std::string userName = splitted.back();
	strcpy(g_setting.userName, userName.c_str());
	splitted.pop_back();
	//scriptAddress
	std::string scriptAddress = splitted.back();
	strcpy(g_setting.scriptAddress, scriptAddress.c_str());
	splitted.pop_back();
	//ver
	std::string ver = splitted.back();
	g_setting.ver = atoi(ver.c_str());
	splitted.pop_back();
	//////////////////////////////////////////////////////////////////////
}