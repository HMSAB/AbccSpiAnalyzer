/******************************************************************************
**  Copyright (C) 2015-2017 HMS Industrial Networks Inc, all rights reserved
*******************************************************************************
**
**       File: AbccSpiAnalyzerSettings.cpp
**    Summary: DLL-Settings source
**     Author: Jon Carrier
**
*******************************************************************************
******************************************************************************/

#include "AbccSpiAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <sstream>
#include <cstring>

/* Anytime behavior or definition of settings change, increment this counter. */
#define SETTINGS_REVISION_STRING "REVISION_00000001"

/* Default setting states */
static const U32  d_MessageIndexingVerbosityLevel = e_VERBOSITY_LEVEL_DETAILED;
static const U32  d_MsgDataPriority               = e_MSG_DATA_PRIORITIZE_TAG;
static const U32  d_ProcessDataPriority           = e_PROCESS_DATA_PRIORITIZE_TAG;
static const U32  d_TimestampIndexing             = e_TIMESTAMP_DISABLED;
static const bool d_MessageSrcIdIndexing          = true;
static const bool d_ErrorIndexing                 = true;
static const bool d_AnybusStatusIndexing          = true;
static const bool d_ApplStatusIndexing            = true;

SpiAnalyzerSettings::SpiAnalyzerSettings()
	: mMosiChannel(UNDEFINED_CHANNEL),
	mMisoChannel(UNDEFINED_CHANNEL),
	mClockChannel(UNDEFINED_CHANNEL),
	mEnableChannel(UNDEFINED_CHANNEL),
	mMessageIndexingVerbosityLevel(d_MessageIndexingVerbosityLevel),
	mMsgDataPriority(d_MsgDataPriority),
	mProcessDataPriority(d_ProcessDataPriority),
	mMessageSrcIdIndexing(d_MessageSrcIdIndexing),
	mErrorIndexing(d_ErrorIndexing),
	mTimestampIndexing(d_TimestampIndexing),
	mAnybusStatusIndexing(d_AnybusStatusIndexing),
	mApplStatusIndexing(d_ApplStatusIndexing),
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

	AddInterface(mMosiChannelInterface.get());
	AddInterface(mMisoChannelInterface.get());
	AddInterface(mClockChannelInterface.get());
	AddInterface(mEnableChannelInterface.get());

	AddInterface(mIndexErrorsInterface.get());
	AddInterface(mIndexTimestampsInterface.get());
	AddInterface(mIndexAnybusStatusInterface.get());
	AddInterface(mIndexApplStatusInterface.get());
	AddInterface(mIndexMessageSrcIdInterface.get());
	AddInterface(mMessageIndexingVerbosityLevelInterface.get());
	AddInterface(mMsgDataPriorityInterface.get());
	AddInterface(mProcessDataPriorityInterface.get());

	AddExportOption(0, "Export All Frame Data");
	AddExportExtension(0, "All Frame Data", "csv");
	AddExportOption(1, "Export Message Data");
	AddExportExtension(1, "Message Data", "csv");
	AddExportOption(2, "Export Process Data");
	AddExportExtension(2, "Process Data", "csv");

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

	mMessageIndexingVerbosityLevel = U32(mMessageIndexingVerbosityLevelInterface->GetNumber());
	mMsgDataPriority               = U32(mMsgDataPriorityInterface->GetNumber());
	mProcessDataPriority           = U32(mProcessDataPriorityInterface->GetNumber());
	mMessageSrcIdIndexing          = bool(mIndexMessageSrcIdInterface->GetValue());
	mErrorIndexing                 = bool(mIndexErrorsInterface->GetValue());
	mTimestampIndexing             = U32(mIndexTimestampsInterface->GetNumber());
	mAnybusStatusIndexing          = bool(mIndexAnybusStatusInterface->GetValue());
	mApplStatusIndexing            = bool(mIndexApplStatusInterface->GetValue());

	ClearChannels();
	AddChannel(mMosiChannel,   "MOSI",   mMosiChannel   != UNDEFINED_CHANNEL);
	AddChannel(mMisoChannel,   "MISO",   mMisoChannel   != UNDEFINED_CHANNEL);
	AddChannel(mClockChannel,  "CLOCK",  mClockChannel  != UNDEFINED_CHANNEL);
	AddChannel(mEnableChannel, "ENABLE", mEnableChannel != UNDEFINED_CHANNEL);

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
		text_archive >> mMessageIndexingVerbosityLevel;
		text_archive >> mMsgDataPriority;
		text_archive >> mProcessDataPriority;
		text_archive >> mMessageSrcIdIndexing;
		text_archive >> mErrorIndexing;
		text_archive >> mTimestampIndexing;
		text_archive >> mAnybusStatusIndexing;
		text_archive >> mApplStatusIndexing;
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
	text_archive << mMessageIndexingVerbosityLevel;
	text_archive << mMsgDataPriority;
	text_archive << mProcessDataPriority;
	text_archive << mMessageSrcIdIndexing;
	text_archive << mErrorIndexing;
	text_archive << mTimestampIndexing;
	text_archive << mAnybusStatusIndexing;
	text_archive << mApplStatusIndexing;

	SaveSettingChangeID();

	return SetReturnString(text_archive.GetString());
}

void SpiAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mMosiChannelInterface->SetChannel(mMosiChannel);
	mMisoChannelInterface->SetChannel(mMisoChannel);
	mClockChannelInterface->SetChannel(mClockChannel);
	mEnableChannelInterface->SetChannel(mEnableChannel);

	mMessageIndexingVerbosityLevelInterface->SetNumber(mMessageIndexingVerbosityLevel);
	mMsgDataPriorityInterface->SetNumber(mMsgDataPriority);
	mProcessDataPriorityInterface->SetNumber(mProcessDataPriority);
	mIndexMessageSrcIdInterface->SetValue(mMessageSrcIdIndexing);
	mIndexErrorsInterface->SetValue(mErrorIndexing);
	mIndexTimestampsInterface->SetNumber(mTimestampIndexing);
	mIndexAnybusStatusInterface->SetValue(mAnybusStatusIndexing);
	mIndexApplStatusInterface->SetValue(mApplStatusIndexing);
}

U8 SpiAnalyzerSettings::SaveSettingChangeID( void )
{
	mChangeID++;
	return mChangeID;
}
