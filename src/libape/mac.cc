/*
 * mac.cc
 *
 *  Created on: Dec 10, 2014
 *      Author: Andrey Karpenko
 */




#define BUILD_CROSS_PLATFORM
#include <mac/All.h>
#include <mac/MACLib.h>
#include <mac/APETag.h>
#include <mac/APEInfo.h>
#include <mac/CharacterHelper.h>


#include <libaudcore/audstrings.h>
#include <libaudcore/runtime.h>
#include <libaudcore/i18n.h>
#include <libaudcore/plugin.h>
#include <libaudcore/preferences.h>
#include <audacious/audtag.h>

class APEPlugin : public InputPlugin
{
public:
    static const char about[];
    static const char * const exts[];
    static constexpr PluginInfo info = {
        N_("Monkey's audio plugin"),
        PACKAGE,
        about
    };


    static constexpr auto iinfo = InputInfo ( 0 )
        .with_exts (exts);

    constexpr APEPlugin() : InputPlugin (info, iinfo) {}

    bool init ();
    void cleanup ();

    bool is_our_file (const char * filename, VFSFile & file);
    Tuple read_tuple (const char * filename, VFSFile & file);
    bool play (const char * filename, VFSFile & file);
};

EXPORT APEPlugin aud_plugin_instance;

const char APEPlugin::about[] =
 N_("Original code by\n"
    "Andrey Karpenko <andrey@delfa.net>\n\n"
    "http://www.borinstruments.delfa.net/");

/* plugin description header */
const char * const APEPlugin::exts[] = { "mac", "ape", "apl", nullptr };


bool APEPlugin::init ()
{
    AUDDBG("Initializing libape library\n");

    return true;
}

void APEPlugin::cleanup ()
{
    AUDDBG("Deinitializing libape library\n");
}


class VFSCIO : public CIO
{
public:

    // construction / destruction
	VFSCIO();
    ~VFSCIO();

    // open / close
    int Open(const wchar_t* pName, BOOL bOpenReadOnly = false);
    int Close();

    // read / write
    int Read(void * pBuffer, unsigned int nBytesToRead, unsigned int * pBytesRead);
    int Write(const void * pBuffer, unsigned int nBytesToWrite, unsigned int * pBytesWritten);

    // seek
    int Seek(int nDistance, unsigned int nMoveMode);

    // other functions
    int SetEOF();

    // creation / destruction
    int Create(const wchar_t * pName);
    int Delete();

    // attributes
    int GetPosition();
    int GetSize();
    int GetName(wchar_t * pBuffer);

    void setvfs(VFSFile *svfs);
private:
    VFSFile * vfs;
    bool 	opened;
};


VFSCIO::VFSCIO()
{
	vfs = NULL;
	opened = false;
}
VFSCIO::~VFSCIO()
{
	Close();
}

int VFSCIO::Open(const wchar_t* pName, BOOL bOpenReadOnly)
{
/*	if(openfunc)
	{
		vfs = new VFSFile((char*)(CAPECharacterHelper::GetUTF8FromUTF16(pName)),"rb");
		if(vfs)return true;
	}

	vfs = vfs_fopen ((char*)(CAPECharacterHelper::GetUTF8FromUTF16(pName)),"rb");
	if(vfs)return true;*/
	return false;
}

int VFSCIO::Close()
{
	if(opened && vfs)
	{
//		return (*closefunc)(vfs);
		//return vfs->fflush();
	}

	return true;
}

int VFSCIO::Read(void * pBuffer, unsigned int nBytesToRead, unsigned int * pBytesRead)
{
	if(vfs!=0)
	{
		int64_t cur = vfs-> ftell();
		*pBytesRead = (unsigned int)vfs->fread(pBuffer, 1, nBytesToRead);
		if(nBytesToRead > *pBytesRead)
		{
			if(!vfs ->feof())
			{
				AUDERR("Requested %d bytes, actually read %d bytes\n", nBytesToRead, *pBytesRead);
				AUDERR("Trying to retry from the same place\n");
				if(!vfs -> fseek(cur, VFSSeekType::VFS_SEEK_SET))
				{
					*pBytesRead = (unsigned int)vfs->fread(pBuffer, 1, nBytesToRead);
					AUDERR("After second attempt read %d bytes\n", *pBytesRead);
				}
			}
		}
		return 0;
	}
	return 1;
}

int VFSCIO::Seek(int nDistance, unsigned int nMoveMode)
{
	if(vfs!=0)
	{
		/*switch(nMoveMode)
		{
		case FILE_BEGIN:
			return ( vfs->fseek (nDistance, VFSSeekType::VFS_SEEK_SET));
			break;
		case FILE_CURRENT:
			return ( vfs->fseek (nDistance, VFSSeekType::VFS_SEEK_CUR));
			break;
		case FILE_END:
			return ( vfs->fseek (nDistance, VFSSeekType::VFS_SEEK_END));
			break;
		}*/
		return ( vfs->fseek (nDistance, (VFSSeekType)nMoveMode));
	}
	return 1;
}

int VFSCIO::Write(const void * pBuffer, unsigned int nBytesToWrite, unsigned int * pBytesWritten)
{
	return 1;
}

int VFSCIO::SetEOF()
{
	return false;
}

// creation / destruction
int VFSCIO::Create(const wchar_t * pName)
{
	return false;
}

int VFSCIO::Delete()
{
	return false;
}

// attributes
int VFSCIO::GetPosition()
{
	return (int)vfs->ftell();
}

int VFSCIO::GetSize()
{
    int nCurrentPosition = GetPosition();
    Seek(0, FILE_END);
    int nLength = GetPosition();
    Seek(nCurrentPosition, FILE_BEGIN);
    return nLength;
}

int VFSCIO::GetName(wchar_t * pBuffer)
{
	return false;
}

void VFSCIO::setvfs(VFSFile *svfs)
{
	if(opened)
	{
		Close();
		opened = false;
	}
	vfs = svfs;
	opened = true;
}




bool APEPlugin::is_our_file (const char * fname, VFSFile & file)
{
	if (! file)
		return FALSE;

	IAPEDecompress* APED;
	int error;

	VFSCIO vfs;
	vfs.setvfs(&file);
	APED = CreateIAPEDecompressEx(static_cast<CIO*>(&vfs), &error);

	if(APED == NULL)
	{
		AUDERR("APED == NULL error unable to create APED class %d\n",error);
		return FALSE;
	}else
	{
		delete(APED);
	}

	return TRUE;
}

Tuple APEPlugin::read_tuple (const char * filename, VFSFile & file)
{
	bool stream = (file.fsize () < 0);

	IAPEDecompress* APED;
	int error;
	VFSCIO vfs;
	vfs.setvfs(&file);
	APED = CreateIAPEDecompressEx(static_cast<CIO*>(&vfs), &error);

	if(APED == NULL)
	{
		AUDERR("APED == NULL error unable to create APED class %d\n",error);
		return Tuple();
	}

	Tuple tuple;
	tuple.set_filename (filename);
	tuple.set_str( Tuple::Codec,  "Monkey's Audio");
	tuple.set_str( Tuple::Quality, "Loss less");
	tuple.set_int( Tuple::Bitrate, APED->GetInfo(APE_DECOMPRESS_AVERAGE_BITRATE) );
	tuple.set_int( Tuple::Length, APED->GetInfo(APE_INFO_LENGTH_MS) );

	delete APED;

	if (! stream && ! file.fseek(0, (VFSSeekType)SEEK_SET))
		audtag::tuple_read (tuple, file);

	if (stream)
		tuple.fetch_stream_info (file);

	return tuple;
}

typedef struct {
	VFSFile* fd;
	IAPEDecompress* pAPEDecompress;
	bool stream;
	Tuple tu;

	// media information
	long samplerate;
	int channels;
	unsigned int bits_per_sample;
	unsigned int length_in_ms;
	unsigned int block_align;
	int sample_format;
} APEPlaybackContext;


bool APEPlugin::play (const char * filename, VFSFile & file)
{
	bool error = FALSE;
	APEPlaybackContext ctx;
	int ret;
    Index<char> outbuf;


	int bitrate = 0;
	int error_count = 0;

	memset(&ctx, 0, sizeof(APEPlaybackContext));

	AUDDBG("APE playback worker started for %s\n", filename);
	ctx.fd = &file;

	AUDDBG ("Checking for streaming ...\n");
	ctx.stream = (file.fsize () < 0);
	ctx.tu = ctx.stream ? get_playback_tuple () : Tuple ();

	VFSCIO vfs;
	vfs.setvfs(&file);
	ctx.pAPEDecompress = CreateIAPEDecompressEx(static_cast<CIO*>(&vfs), &ret);

	if (ctx.pAPEDecompress == 0)
	{
		AUDERR(" Unable to open:[%s]\nAPE: error number: %d\n", filename, ret);
		error = TRUE;
		goto cleanup;
	}
	ctx.block_align = ctx.pAPEDecompress->GetInfo(APE_INFO_BLOCK_ALIGN);
	outbuf.resize(1024 * ctx.block_align);

	ctx.samplerate = ctx.pAPEDecompress->GetInfo(APE_INFO_SAMPLE_RATE);
	ctx.channels = ctx.pAPEDecompress->GetInfo(APE_INFO_CHANNELS);
	ctx.bits_per_sample = ctx.pAPEDecompress->GetInfo(APE_INFO_BITS_PER_SAMPLE);

	AUDDBG("Block align %d Sample Rate %d Channels %d Bits Per Sample %d\n",(int)ctx.block_align, (int)ctx.samplerate,\
			(int)ctx.channels, (int)ctx.bits_per_sample);

	int blocksread;
	if( ctx.pAPEDecompress->GetData( outbuf.begin(), 1024, &blocksread ) != 0)
	{
		AUDERR("Unable to decode first block\n");
		error = TRUE;
		goto cleanup;
	}

	bitrate = ctx.channels * ctx.bits_per_sample * ctx.samplerate;
	set_stream_bitrate  (bitrate);


	switch(ctx.block_align/ctx.channels)
	{
	case 1:
		ctx.sample_format = FMT_S8;
		break;
	case 2:
		ctx.sample_format = FMT_S16_LE;
		break;
	case 4:
		ctx.sample_format = FMT_S32_LE;
		break;
	default:
		AUDERR("Can't select Sample Format\n");
		error = TRUE;
		goto cleanup;
	}

	open_audio (ctx.sample_format, ctx.samplerate, ctx.channels);

	while ( !check_stop() )
	{
		int seek = check_seek ();

		if (seek >= 0)
		{
			if (ctx.pAPEDecompress->Seek( (int64_t) seek * ctx.samplerate / 1000 ) != 0)
			{
				AUDERR("Seek error\n");
				error = TRUE;
				goto cleanup;
			}
		}

		ret = ctx.pAPEDecompress->GetData(outbuf.begin(), 1024, &blocksread);
		if ( blocksread == 0 || ret  != 0 )
		{
			if (ret == 0 )
				break;

			if (++ error_count >= 10)
			{
				AUDERR("Unable to decode data (more then 10 retries)\n");
				error = true;
				break;
			}
		}
		else
		{
			error_count = 0;
			write_audio (outbuf.begin(), blocksread*ctx.block_align);
		}
	}

cleanup:
	delete(ctx.pAPEDecompress);
	return ! error;
}
