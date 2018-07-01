/******************************************************************************
**  Copyright (C) 2015-2018 HMS Industrial Networks Inc, all rights reserved
*******************************************************************************
**
**       File: AbccSpiAnalyzerSettings.cpp
**    Summary: DLL-Settings source
**     Author: Jon Carrier
**
*******************************************************************************
******************************************************************************/

#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <locale>

#include "rapidxml-1.13/rapidxml.hpp"

#include "AbccSpiAnalyzerSettings.h"
#include "AnalyzerHelpers.h"

/* Anytime behavior or definition of settings change, increment this counter. */
#define SETTINGS_REVISION_STRING "REVISION_00000004"

/* Default setting states */
static const U32  d_MessageIndexingVerbosityLevel = e_VERBOSITY_LEVEL_DETAILED;
static const U32  d_MsgDataPriority               = e_MSG_DATA_PRIORITIZE_TAG;
static const U32  d_ProcessDataPriority           = e_PROCESS_DATA_PRIORITIZE_TAG;
static const U32  d_TimestampIndexing             = e_TIMESTAMP_DISABLED;
static const bool d_MessageSrcIdIndexing          = true;
static const bool d_ErrorIndexing                 = true;
static const bool d_AnybusStatusIndexing          = true;
static const bool d_ApplStatusIndexing            = true;
static const char* d_AdvSettingsPath              = "";

/* Forward declarations */
static inline void TrimLeft(std::string &s);
static inline void TrimRight(std::string &s);
static inline void TrimString(std::string &s);

SpiAnalyzerSettings::SpiAnalyzerSettings()
	: mMosiChannel(UNDEFINED_CHANNEL),
	mMisoChannel(UNDEFINED_CHANNEL),
	mClockChannel(UNDEFINED_CHANNEL),
	mEnableChannel(UNDEFINED_CHANNEL),
	mNetworkType(e_NW_TYPE_UNSPECIFIED),
	mMessageIndexingVerbosityLevel(d_MessageIndexingVerbosityLevel),
	mMsgDataPriority(d_MsgDataPriority),
	mProcessDataPriority(d_ProcessDataPriority),
	mMessageSrcIdIndexing(d_MessageSrcIdIndexing),
	mErrorIndexing(d_ErrorIndexing),
	mTimestampIndexing(d_TimestampIndexing),
	mAnybusStatusIndexing(d_AnybusStatusIndexing),
	mApplStatusIndexing(d_ApplStatusIndexing),
#if ENABLE_ADVANCED_SETTINGS
	mAdvSettingsPath(d_AdvSettingsPath),
	m3WireOn4Channels(false),
	m4WireOn3Channels(false),
#endif
	mChangeID(0)
{
	mMosiChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mMosiChannelInterface->SetTitleAndTooltip("SPI Channel - MOSI :", "Master Out, Slave In (Host to Module)");
	mMosiChannelInterface->SetChannel(mMosiChannel);

	mMisoChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mMisoChannelInterface->SetTitleAndTooltip("SPI Channel - MISO :", "Master In, Slave Out (Module to Host)");
	mMisoChannelInterface->SetChannel(mMisoChannel);

	mClockChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mClockChannelInterface->SetTitleAndTooltip("SPI Channel - Clock :", "Clock (CLK)");
	mClockChannelInterface->SetChannel(mClockChannel);

	mEnableChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
	mEnableChannelInterface->SetTitleAndTooltip("SPI Channel - Enable :", "Enable (SS, Slave Select)");
	mEnableChannelInterface->SetChannel(mEnableChannel);
	mEnableChannelInterface->SetSelectionOfNoneIsAllowed(true);


	mNetworkTypeInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mNetworkTypeInterface->SetTitleAndTooltip("Network Type :", "Used to process network specific details such as the network configuration object's instance names.\nCan be set to \"Unspecified\", if unsure or if such details are not important.");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_UNSPECIFIED,  "Unspecified","");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_PDPV0,  "PROFIBUS DP-V0", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_PDPV1,  "PROFIBUS DP-V1", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_COP,  "CANopen", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_DEV,  "DeviceNet", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_RTU,  "Modbus-RTU", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_CNT,  "ControlNet", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_ETN_1P,  "Modbus-TCP", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_PRT,  "PROFINET RT", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_EIP_1P,  "EtherNet/IP", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_ECT,  "EtherCAT", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_PIR,  "PROFINET IRT", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_CCL,  "CC-Link", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_ETN_2P,  "Modbus-TCP 2-Port", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_CPN,  "CompoNet", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_PRT_2P,  "PROFINET RT 2-port", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_SRC3,  "SERCOS III", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_BMP,  "BACnet MS/TP", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_BIP,  "BACnet/IP", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_EIP_2P_BB,  "EtherNet/IP 2-Port BB DLR", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_EIP_2P,  "EtherNet/IP 2-Port", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_PIR_FO,  "PROFINET IRT FO", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_EPL,  "POWERLINK", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_CFN,  "CC-Link IE Field Network", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_CET,  "Common Ethernet", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_EIP_2P_BB_IIOT,  "EtherNet/IP IIoT", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_PIR_IIOT,  "PROFINET IRT IIoT", "");
	mNetworkTypeInterface->AddNumber(e_NW_TYPE_PIR_FO_IIOT,  "PROFINET IRT FO IIoT", "");
	mNetworkTypeInterface->SetNumber(mNetworkType);

	mIndexErrorsInterface.reset(new AnalyzerSettingInterfaceBool());
	mIndexErrorsInterface->SetTitleAndTooltip("Index - Errors :", "Enable indexed searching of errors.");
	mIndexErrorsInterface->SetValue(mErrorIndexing);

	mIndexAnybusStatusInterface.reset(new AnalyzerSettingInterfaceBool());
	mIndexAnybusStatusInterface->SetTitleAndTooltip("Index - Anybus Status :", "Enable indexed searching of Anybus status.\nEntries are added on status change events.\nFirst valid packet in capture will always trigger this event.");
	mIndexAnybusStatusInterface->SetValue(mAnybusStatusIndexing);

	mIndexApplStatusInterface.reset(new AnalyzerSettingInterfaceBool());
	mIndexApplStatusInterface->SetTitleAndTooltip("Index - Application Status :", "Enable indexed searching of application status.\nEntries are added on status change events.\nFirst valid packet in capture will always trigger this event.");
	mIndexApplStatusInterface->SetValue(mApplStatusIndexing);

	mIndexTimestampsInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mIndexTimestampsInterface->SetTitleAndTooltip("Index - Network Timestamp :", "Enable indexed searching of ABCC network timestamps.\nUseful for benchmarking; otherwise it is recommended to keep this option disabled.");
	mIndexTimestampsInterface->AddNumber(e_TIMESTAMP_DISABLED,  "Disabled", "No network timestamps will be added to tabular results.");
	mIndexTimestampsInterface->AddNumber(e_TIMESTAMP_ALL_PACKETS,  "All Packets", "The timestamp from every ABCC SPI packet will be added to tabular results.");
	mIndexTimestampsInterface->AddNumber(e_TIMESTAMP_WRITE_PROCESS_DATA_VALID, "Write Process Data Valid", "The timestamp from ABCC SPI packets containing \"valid\" write process data will be added to tabular results.");
	mIndexTimestampsInterface->AddNumber(e_TIMESTAMP_NEW_READ_PROCESS_DATA, "New Read Process Data", "The timestamp from ABCC SPI packets containing \"new\" read process data will be added to tabular results.");
	mIndexTimestampsInterface->SetNumber(mTimestampIndexing);

	mIndexMessageSrcIdInterface.reset(new AnalyzerSettingInterfaceBool());
	mIndexMessageSrcIdInterface->SetTitleAndTooltip("Index - Message 'Source ID' :", "Enable indexed searching of the source ID associated with an ABCC transaction.");
	mIndexMessageSrcIdInterface->SetValue(mMessageSrcIdIndexing);

	mMessageIndexingVerbosityLevelInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mMessageIndexingVerbosityLevelInterface->SetTitleAndTooltip("Index - Message :", "Specifies how detailed the decoded protcols entries are.");
	mMessageIndexingVerbosityLevelInterface->AddNumber(e_VERBOSITY_LEVEL_DISABLED,  "Disabled", "Messages will not be indexed in searchable results.\nUse when object messaging is of no interest.");
	mMessageIndexingVerbosityLevelInterface->AddNumber(e_VERBOSITY_LEVEL_COMPACT,  "Compact Results", "Message header information is added to a single tabular result.\nThis option is useful when looking for very specific messages.");
	mMessageIndexingVerbosityLevelInterface->AddNumber(e_VERBOSITY_LEVEL_DETAILED, "Verbose Results", "Message header information is added to tabular results individually.\nRecommended setting for general use.");
	mMessageIndexingVerbosityLevelInterface->SetNumber(mMessageIndexingVerbosityLevel);

	mMsgDataPriorityInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mMsgDataPriorityInterface->SetTitleAndTooltip("Message Data Priority :", "Specifies if the Message Data or Tag information is given priority in the display of multi-layered bubble-text.");
	mMsgDataPriorityInterface->AddNumber(e_MSG_DATA_PRIORITIZE_DATA, "Prioritize Data", "Message Data will be displayed as first layer of bubble text in analyzer results.");
	mMsgDataPriorityInterface->AddNumber(e_MSG_DATA_PRIORITIZE_TAG, "Prioritize Tag", "Message Data will be displayed as second layer of bubble text in analyzer results.");
	mMsgDataPriorityInterface->SetNumber(mMsgDataPriority);

	mProcessDataPriorityInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mProcessDataPriorityInterface->SetTitleAndTooltip("Process Data Priority :", "Specifies if the Process Data or Tag information is given priority in the display of multi-layered bubble-text.");
	mProcessDataPriorityInterface->AddNumber(e_MSG_DATA_PRIORITIZE_DATA, "Prioritize Data", "Process Data will be displayed as first layer of bubble text in analyzer results.");
	mProcessDataPriorityInterface->AddNumber(e_MSG_DATA_PRIORITIZE_TAG, "Prioritize Tag", "Process Data will be displayed as second layer of bubble text in analyzer results.");
	mProcessDataPriorityInterface->SetNumber(mProcessDataPriority);

#if ENABLE_ADVANCED_SETTINGS
	mAdvancedSettingsInterface.reset(new AnalyzerSettingInterfaceText());
	mAdvancedSettingsInterface->SetTextType(AnalyzerSettingInterfaceText::FilePath);
	mAdvancedSettingsInterface->SetTitleAndTooltip("Advanced Settings :", "Specifies external settings file to control special (advanced) settings of the plugin.\nPlease refer to plugin documentation for more details.\nIf left empty plugin defaults will be used which are suitable for most situations.");
	mAdvancedSettingsInterface->SetText(mAdvSettingsPath);
#endif

	AddInterface(mMosiChannelInterface.get());
	AddInterface(mMisoChannelInterface.get());
	AddInterface(mClockChannelInterface.get());
	AddInterface(mEnableChannelInterface.get());

	AddInterface(mNetworkTypeInterface.get());
	AddInterface(mIndexErrorsInterface.get());
	AddInterface(mIndexTimestampsInterface.get());
	AddInterface(mIndexAnybusStatusInterface.get());
	AddInterface(mIndexApplStatusInterface.get());
	AddInterface(mIndexMessageSrcIdInterface.get());
	AddInterface(mMessageIndexingVerbosityLevelInterface.get());
	AddInterface(mMsgDataPriorityInterface.get());
	AddInterface(mProcessDataPriorityInterface.get());
#if ENABLE_ADVANCED_SETTINGS
	AddInterface(mAdvancedSettingsInterface.get());
#endif

	AddExportOption(e_EXPORT_FRAMES, "Export All Frame Data");
	AddExportExtension(e_EXPORT_FRAMES, "All Frame Data", "csv");
	AddExportOption(e_EXPORT_PROCESS_DATA, "Export Process Data");
	AddExportExtension(e_EXPORT_PROCESS_DATA, "Process Data", "csv");
	AddExportOption(e_EXPORT_MESSAGE_DATA, "Export Message Data");
	AddExportExtension(e_EXPORT_MESSAGE_DATA, "Message Data", "csv");

	//AddExportOption(3, "Export as XML file");
	//AddExportExtension(3, "XML-File", "xml");

	ClearChannels();
	AddChannel(mMosiChannel, "MOSI", false);
	AddChannel(mMisoChannel, "MISO", false);
	AddChannel(mClockChannel, "CLOCK", false);
	AddChannel(mEnableChannel, "ENABLE", false);
}

SpiAnalyzerSettings::~SpiAnalyzerSettings()
{
}

bool SpiAnalyzerSettings::ParseAdavancedSettingsFile(void)
{
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<> * root_node;
	std::string trimmedPath(mAdvSettingsPath);
	std::string type;
	std::string value;

	/*
	** Trim the file path so that an entry with nothing
	** but whitespace is ignored as a valid file path
	*/
	TrimString(trimmedPath);

	/* Read the xml file into a vector */
	if( trimmedPath.length() > 0 )
	{
		std::ifstream filestream(trimmedPath);

		if(!filestream)
		{
			SetErrorText("Advanced settings: File not found or could not be opened.");
			return false;
		}
		else
		{
			std::vector<char> buffer((std::istreambuf_iterator<char>(filestream)), std::istreambuf_iterator<char>());
			buffer.push_back('\0');

			/* Parse the buffer using the xml file parsing library into doc */
			doc.parse<0>(&buffer[0]);

			/* Jump to root node */
			root_node = doc.first_node("AdvancedSettings");

			if (root_node)
			{
				/* Iterate over each available setting */
				for (rapidxml::xml_node<> * settings_node = root_node->first_node("Setting"); settings_node; settings_node = settings_node->next_sibling())
				{
					rapidxml::xml_attribute<char>* ptr = settings_node->first_attribute("type");

					/* Get the type of setting */
					if (ptr != nullptr)
					{
						type = ptr->value();

						if (type.compare("3-wire-on-4-channels") == 0)
						{
							value = settings_node->value();
							if (value.compare("1") == 0)
							{
								m3WireOn4Channels = true;
							}
							else
							{
								m3WireOn4Channels = false;
							}
						}
						else if (type.compare("4-wire-on-3-channels") == 0)
						{
							value = settings_node->value();
							if (value.compare("1") == 0)
							{
								m4WireOn3Channels = true;
							}
							else
							{
								m4WireOn3Channels = false;
							}
						}
					}
				}

				if (m4WireOn3Channels && m3WireOn4Channels)
				{
					m4WireOn3Channels = false;
					m3WireOn4Channels = false;
					SetErrorText("Advanced settings: 4-wire-on-3-channels and 3-wire-on-4-channels are mutually exclusive features, both cannot be enabled simultaneously.\r\nPlease fix the configuration.");

					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				SetErrorText("Advanced settings: File could not be properly parsed.");
				return false;
			}
		}
	}
	else
	{
		return true;
	}
}

bool SpiAnalyzerSettings::SetSettingsFromInterfaces()
{
	Channel mosi   = mMosiChannelInterface->GetChannel();
	Channel miso   = mMisoChannelInterface->GetChannel();
	Channel clock  = mClockChannelInterface->GetChannel();
	Channel enable = mEnableChannelInterface->GetChannel();

	std::vector<Channel> channels;
	channels.push_back(mosi);
	channels.push_back(miso);
	channels.push_back(clock);
	channels.push_back(enable);

	if (AnalyzerHelpers::DoChannelsOverlap(&channels[0], (U32)channels.size()) == true)
	{
		SetErrorText("Please select different channels for each input.");
		return false;
	}

	if ((mosi == UNDEFINED_CHANNEL) || (miso == UNDEFINED_CHANNEL) || (clock == UNDEFINED_CHANNEL))
	{
		SetErrorText("Please select a channel for MOSI, MISO, SCLK.");
		return false;
	}

	mMosiChannel   = mMosiChannelInterface->GetChannel();
	mMisoChannel   = mMisoChannelInterface->GetChannel();
	mClockChannel  = mClockChannelInterface->GetChannel();
	mEnableChannel = mEnableChannelInterface->GetChannel();

	mNetworkType                   = U32(mNetworkTypeInterface->GetNumber());
	mMessageIndexingVerbosityLevel = U32(mMessageIndexingVerbosityLevelInterface->GetNumber());
	mMsgDataPriority               = U32(mMsgDataPriorityInterface->GetNumber());
	mProcessDataPriority           = U32(mProcessDataPriorityInterface->GetNumber());
	mMessageSrcIdIndexing          = bool(mIndexMessageSrcIdInterface->GetValue());
	mErrorIndexing                 = bool(mIndexErrorsInterface->GetValue());
	mTimestampIndexing             = U32(mIndexTimestampsInterface->GetNumber());
	mAnybusStatusIndexing          = bool(mIndexAnybusStatusInterface->GetValue());
	mApplStatusIndexing            = bool(mIndexApplStatusInterface->GetValue());
	#if ENABLE_ADVANCED_SETTINGS
	mAdvSettingsPath               = mAdvancedSettingsInterface->GetText();
	#endif

	ClearChannels();
	AddChannel(mMosiChannel,   "MOSI",   mMosiChannel   != UNDEFINED_CHANNEL);
	AddChannel(mMisoChannel,   "MISO",   mMisoChannel   != UNDEFINED_CHANNEL);
	AddChannel(mClockChannel,  "CLOCK",  mClockChannel  != UNDEFINED_CHANNEL);
	AddChannel(mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL);
#if ENABLE_ADVANCED_SETTINGS
	mAdvSettingsPath = mAdvancedSettingsInterface->GetText();
	if (!ParseAdavancedSettingsFile())
	{
		return false;
	}
#endif
	return true;
}

void SpiAnalyzerSettings::LoadSettings(const char* settings)
{
	SimpleArchive text_archive;
	const char* pcPluginName;
	const char* pcSettingsVersionString;

	text_archive.SetString(settings);

	/* The first thing in the archive is the name of the protocol analyzer that the data belongs to. */
	text_archive >> &pcPluginName;
	if (strcmp(pcPluginName, "AbccSpiAnalyzer") != 0)
	{
		AnalyzerHelpers::Assert("AbccSpiAnalyzer: Provided with a settings string that doesn't belong to us.");
	}

	text_archive >> mMosiChannel;
	text_archive >> mMisoChannel;
	text_archive >> mClockChannel;
	text_archive >> mEnableChannel;

	/* Compare version in archive to what the plugin's "settings" version is */
	text_archive >> &pcSettingsVersionString;
	if (strcmp(pcSettingsVersionString, SETTINGS_REVISION_STRING) == 0)
	{
		text_archive >> mNetworkType;
		text_archive >> mMessageIndexingVerbosityLevel;
		text_archive >> mMsgDataPriority;
		text_archive >> mProcessDataPriority;
		text_archive >> mMessageSrcIdIndexing;
		text_archive >> mErrorIndexing;
		text_archive >> mTimestampIndexing;
		text_archive >> mAnybusStatusIndexing;
		text_archive >> mApplStatusIndexing;
		#if ENABLE_ADVANCED_SETTINGS
		text_archive >> &mAdvSettingsPath;
		#endif
	}

	ClearChannels();
	AddChannel(mMosiChannel,   "MOSI",   mMosiChannel   != UNDEFINED_CHANNEL);
	AddChannel(mMisoChannel,   "MISO",   mMisoChannel   != UNDEFINED_CHANNEL);
	AddChannel(mClockChannel,  "CLOCK",  mClockChannel  != UNDEFINED_CHANNEL);
	AddChannel(mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL);

	UpdateInterfacesFromSettings();
}

const char* SpiAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << "AbccSpiAnalyzer";
	text_archive << mMosiChannel;
	text_archive << mMisoChannel;
	text_archive << mClockChannel;
	text_archive << mEnableChannel;

	text_archive << SETTINGS_REVISION_STRING;
	text_archive << mNetworkType;
	text_archive << mMessageIndexingVerbosityLevel;
	text_archive << mMsgDataPriority;
	text_archive << mProcessDataPriority;
	text_archive << mMessageSrcIdIndexing;
	text_archive << mErrorIndexing;
	text_archive << mTimestampIndexing;
	text_archive << mAnybusStatusIndexing;
	text_archive << mApplStatusIndexing;
	#if ENABLE_ADVANCED_SETTINGS
	text_archive << mAdvSettingsPath;
	#endif

	SaveSettingChangeID();

	return SetReturnString(text_archive.GetString());
}

void SpiAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mMosiChannelInterface->SetChannel(mMosiChannel);
	mMisoChannelInterface->SetChannel(mMisoChannel);
	mClockChannelInterface->SetChannel(mClockChannel);
	mEnableChannelInterface->SetChannel(mEnableChannel);

	mNetworkTypeInterface->SetNumber(mNetworkType);
	mMessageIndexingVerbosityLevelInterface->SetNumber(mMessageIndexingVerbosityLevel);
	mMsgDataPriorityInterface->SetNumber(mMsgDataPriority);
	mProcessDataPriorityInterface->SetNumber(mProcessDataPriority);
	mIndexMessageSrcIdInterface->SetValue(mMessageSrcIdIndexing);
	mIndexErrorsInterface->SetValue(mErrorIndexing);
	mIndexTimestampsInterface->SetNumber(mTimestampIndexing);
	mIndexAnybusStatusInterface->SetValue(mAnybusStatusIndexing);
	mIndexApplStatusInterface->SetValue(mApplStatusIndexing);
	#if ENABLE_ADVANCED_SETTINGS
	mAdvancedSettingsInterface->SetText(mAdvSettingsPath);
	#endif
}

U8 SpiAnalyzerSettings::SaveSettingChangeID( void )
{
	mChangeID++;
	return mChangeID;
}

/*******************************************************************************
** Static helper routines
********************************************************************************
*/

/* Trim from start (in place) */
static inline void TrimLeft(std::string &s)
{
	s.erase(
		s.begin(),
		std::find_if(
			s.begin(),
			s.end(),
			[](int ch) {
				return !std::isspace(ch);
			}
		)
	);
}

/* Trim from end (in place) */
static inline void TrimRight(std::string &s)
{
	s.erase(
		std::find_if(
			s.rbegin(),
			s.rend(),
			[](int ch) {
				return !std::isspace(ch);
			}
		).base(),
		s.end()
	);
}

/* Trim from both ends (in place) */
static inline void TrimString(std::string &s)
{
	TrimLeft(s);
	TrimRight(s);
}
