/*
*
* �ļ�����	��	PX2NetInitTerm.cpp
*
*/

#include "PX2NetInitTerm.hpp"
#include "PX2Assert.hpp"
#include "PX2Log.hpp"

namespace PX2
{

	//----------------------------------------------------------------------------
	void InitializeNetwork()
	{
#if defined(_WIN32) || defined(WIN32)
		WORD    version = MAKEWORD(2, 2);
		WSADATA data;
		memset(&data, 0, sizeof(WSADATA));
		if (WSAStartup(version, &data) != 0)
		{
			assertion(false, "Failed to initialize network subsystem.\n");
			PX2_LOG_SERVER_ERROR("Failed to initialize network subsystem.");
		}
#endif
	}
	//----------------------------------------------------------------------------
	void TerminateNetwork()
	{
#if defined(_WIN32) || defined(WIN32)
		WSACleanup();
#endif
	}
	//----------------------------------------------------------------------------

}