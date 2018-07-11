/******************************************************************************
**  Copyright (C) 2015-2018 HMS Industrial Networks Inc, all rights reserved
*******************************************************************************
**
**       File: AbccSpiAnalyzerResults.h
**    Summary: DLL-Results header
**     Author: Jon Carrier
**
*******************************************************************************
******************************************************************************/

#ifndef ABCC_SPI_ANALYZER_RESULTS_H
#define ABCC_SPI_ANALYZER_RESULTS_H

#include "AnalyzerResults.h"

#ifndef FORMATTED_STRING_BUFFER_SIZE
#define FORMATTED_STRING_BUFFER_SIZE		256
#endif

enum class ErrorEvent
{
	None,
	RetransmitWarning,
	CrcError,
	SpiFragmentationError
};

/* Indicates a SPI settings error (e.g. CPOL, CPHA, EN Active Hi/Lo) */
#define SPI_ERROR_FLAG						( 1 << 0 )

/* Direction flag. When asserted, MOSI, when de-asserted MISO */
#define SPI_MOSI_FLAG						( 1 << 1 )

/* Indicates the first message in a fragmented message transfer */
#define SPI_MSG_FIRST_FRAG_FLAG				( 1 << 2 )

/* Indicates that message fragmentation is in progress */
#define SPI_MSG_FRAG_FLAG					( 1 << 3 )

/* Event flag to indicate any critical events that are part of the ABCC SPI protocol
** This flag is field-specific.
** This flag is relevant for the following fields (not all supported yet):
**   - SPI_CTL: signals a toggle error (retransmission event)
**   - ANB_STS: signals a Anybus status changed event
**   - SPI_STS: signals a toggle error (retransmission event)
**   - APP_STS: signals an application status changed event
**   - MSG_CMD: signals an error response message
**   - CRC32: signals a checksum error */
#define SPI_PROTO_EVENT_FLAG				( 1 << 5 )

class SpiAnalyzer;
class SpiAnalyzerSettings;

class SpiAnalyzerResults : public AnalyzerResults
{
public:
	SpiAnalyzerResults(SpiAnalyzer* analyzer, SpiAnalyzerSettings* settings);
	virtual ~SpiAnalyzerResults();

	virtual void GenerateBubbleText(U64 frame_index, Channel& channel, DisplayBase display_base);
	virtual void GenerateExportFile(const char* file, DisplayBase display_base, U32 export_type_user_id);

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base);
	virtual void GeneratePacketTabularText(U64 packet_id, DisplayBase display_base);
	virtual void GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base);

	virtual U64 GetFrameIdOfAbccFieldContainedInPacket(U64 packet_index, bool is_mosi_channel, U8 type);

protected: /* functions */
	virtual void StringBuilder(char* tag, char* value, char* verbose, bool alert);
	virtual void StringBuilder(char* tag, char* value, char* verbose, bool alert, bool prioritize_value);
	virtual void TableBuilder(bool is_mosi_channel, char* text, bool alert);

	virtual void BuildSpiCtrlString(U8 val, DisplayBase display_base);
	virtual void BuildSpiStsString(U8 val, DisplayBase display_base);
	virtual bool BuildCmdString(U8 val, U8 obj, DisplayBase display_base);
	virtual void BuildErrorRsp(U8 val, DisplayBase display_base);
	virtual void BuildErrorRsp(U8 nw_type_idx, U8 val, U8 obj, DisplayBase display_base);
	virtual void BuildAbccStatus(U8 val, DisplayBase display_base);
	virtual void BuildApplStatus(U8 val, DisplayBase display_base);
	virtual void BuildIntMask(U8 val, DisplayBase display_base);
	virtual void BuildObjectString(U8 val, DisplayBase display_base);

	virtual void BuildInstString(U8 nw_type_idx, U8 obj, U16 val, DisplayBase display_base);
	virtual void BuildAttrString(U8 obj, U16 inst, U16 val, bool indexed, DisplayBase display_base);

	virtual void ExportAllFramesToFile(const char* file, DisplayBase display_base);
	virtual void ExportMessageDataToFile(const char* file, DisplayBase display_base);
	virtual void ExportProcessDataToFile(const char* file, DisplayBase display_base);

	virtual void AppendCsvMessageEntry(void* file, std::stringstream &ss_csv_head, std::stringstream &ss_csv_body, std::stringstream &ss_csv_tail, ErrorEvent event);
	virtual void AppendCsvSafeString(std::stringstream &ss_csv_data, char* input_data_str);

protected:  /* variables */
	SpiAnalyzerSettings* mSettings;
	SpiAnalyzer* mAnalyzer;
	char acMsgSizeStr[2][FORMATTED_STRING_BUFFER_SIZE];
	char acMsgSrcStr[2][FORMATTED_STRING_BUFFER_SIZE];
	char acMsgObjStr[2][FORMATTED_STRING_BUFFER_SIZE];
	char acMsgInstStr[2][FORMATTED_STRING_BUFFER_SIZE];
	char acMsgCmdStr[2][FORMATTED_STRING_BUFFER_SIZE];
	char acMsgExtStr[2][FORMATTED_STRING_BUFFER_SIZE];
	bool fMsgValid[2];
	bool fMsgErrorRsp[2];
};

#endif /* ABCC_SPI_ANALYZER_RESULTS_H */
