/*
 * Copyright (c) 2015 Andrey Karpenko <andrey@delfa.net>.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include <libaudcore/audstrings.h>
#include <libaudcore/i18n.h>
#include <libaudcore/plugin.h>
#include <libaudcore/probe.h>
#include <libaudcore/runtime.h>


#define LLU_t	long long unsigned int

#define bool_t 	uint32_t
#define FALSE	0
#define TRUE	1



const char * const autocue_exts[] = {"cue", nullptr };


class AutoCueLoader : public PlaylistPlugin
{
public:
    static const char about[];

    static constexpr PluginInfo info = {N_("New Automatic Cue Sheet Plugin"), PACKAGE, about};

    constexpr AutoCueLoader () : PlaylistPlugin (info, autocue_exts, false) {}

    bool load (const char * filename, VFSFile & file, String & title,
     Index<PlaylistAddItem> & items);
};

EXPORT AutoCueLoader aud_plugin_instance;


const char AutoCueLoader::about[] =
 N_("Original code by\n"
    "Andrey Karpenko <andrey@delfa.net>\n\n"
    "http://www.borinstruments.delfa.net/");

char * getsuffix(char * fln)
{
	int32_t i;
	i=0;
	while(fln[i]!='\0')i++;
	while(fln[i]!='.' && i>=0)i--;
	return fln+i+1;
}

bool_t delwindowsrelativepath(char * fln)
{
	int32_t i,s2;
	i=0;
	while(fln[i++]!='\0');

	i-=3;
	if(i<0)return FALSE;
  	bool_t exitfl=TRUE;
    while( i>=0 && exitfl )
    {
   		if(fln[i]=='%' && fln[i+1]=='5' && fln[i+2]=='C')
   		{
   			exitfl=FALSE;
   			s2=i+3;
   		}else
   		{
   			i--;
   		}
   	}
    if(exitfl) return FALSE;
    exitfl=TRUE;
    while( i>=0 && exitfl )
    {
   		if(fln[i]=='/')
   		{
   			exitfl=FALSE;
//   			AUDDBG("s2 %d rel %s\n%s\n",s2,fln+i+1,fln+s2);
   			strcpy(fln+i+1,fln+s2);
   			return TRUE;
   		}else
   		{
   			i--;
   		}
   	}
    return FALSE;
}

int32_t currentsufix=-1;
int suffcnt=0;
char ** dsuffixdb=NULL;


int maxsufflen=0;
bool_t extscaned=FALSE;

void updatesuffix(char * fln,int32_t sfx)
{
	if(sfx!=-1 && fln!=NULL)
	{
		char * t=getsuffix(fln);
//		AUDDBG("old suffix %s new suffix %s\n",t,dsuffixdb[sfx]);
		strcpy(t,dsuffixdb[sfx]);
	}
}

// extension scanner
static bool addext(PluginHandle * plugin/*, void * vdata*/)
{
//	char ** data;
	const char * kk;
//	data = (char**)vdata;
	kk=aud_plugin_get_name(plugin);
	InputPlugin * decoder=(InputPlugin*)aud_plugin_get_header(plugin);
	if(decoder == NULL)
		return true;
	AUDDBG("Plugin name: %s\n",kk);
	int i=0;
	AUDDBG("Plugin extentions:");
	if(decoder->input_info.keys[InputKey::Ext] != NULL)
		while (decoder->input_info.keys[InputKey::Ext][i] != NULL)
		{
			AUDDBG(" %s",decoder->input_info.keys[InputKey::Ext][i]);
			dsuffixdb=(char**)realloc(dsuffixdb,sizeof(char*)*(suffcnt+1));
			if(dsuffixdb==NULL)
			{
				AUDDBG("Out of memory\n");
				return FALSE;
			}

			int ll = strlen( decoder->input_info.keys[InputKey::Ext][i] )+1;
			dsuffixdb[suffcnt]=(char*)malloc(ll);
			if(dsuffixdb[suffcnt]==NULL)
			{
				AUDDBG("Out of memory\n");
				return FALSE;
			}

			if(maxsufflen<ll)maxsufflen=ll;
			strcpy(dsuffixdb[suffcnt++],decoder->input_info.keys[InputKey::Ext][i++]);
		}
	AUDDBG(" \n");
	return TRUE;
}

void DelSuffixdb()
{
	if(suffcnt!=0)
	{
		for(int i=0;i<suffcnt;i++)
		{
			free(dsuffixdb[i]);
		}
		suffcnt=0;
		free(dsuffixdb);
		dsuffixdb=NULL;
	}
    currentsufix=-1;
    extscaned=FALSE;
}

void struppercase( char *sPtr )
{
	if(sPtr!=NULL)
	{
		while( *sPtr != '\0' )
		{
			*sPtr = toupper( ( unsigned char ) *sPtr );
			sPtr++;
		}
	}
}

void ScanEnxtentions()
{
	// Free memory to rescan extensions from the beginning
	//DelSuffixdb();

	if(extscaned==FALSE)
	{
		maxsufflen=0;

		//aud_plugin_for_enabled(PLUGIN_TYPE_INPUT,(PluginForEachFunc)addext,NULL);
		auto & pList = aud_plugin_list (PluginType::Input);

		for (PluginHandle * plugin : pList)
		{
		    if (! aud_plugin_get_enabled (plugin))
		        continue;
		    addext(plugin);

		}


		if(suffcnt!=0)
		{
			// Add upper case extension versions
			dsuffixdb=(char**)realloc(dsuffixdb,sizeof(char*)*(suffcnt*2));
			if(dsuffixdb==NULL)
			{
				AUDDBG("Out of memory\n");
				return;
			}
			for(int i=0;i<suffcnt;i++)
			{
				dsuffixdb[i+suffcnt]=(char*)malloc(sizeof(char)*maxsufflen);
				if(dsuffixdb[i+suffcnt]==NULL)
				{
					AUDDBG("Out of memory\n");
					return;
				}
				strcpy(dsuffixdb[i+suffcnt],dsuffixdb[i]);
				struppercase(dsuffixdb[i+suffcnt]);
			}
			suffcnt*=2;

			AUDDBG("suffix max len %llu:\n",(LLU_t)maxsufflen);
			for(int i=0;i<suffcnt;i++)
			{
				AUDDBG("%s\n",dsuffixdb[i]);
			}
		}
	}
	extscaned=TRUE;
}

char * relocatemem(char * filename)
{
	// allocate memory for longer file name
	char * tmpfn = (char*)malloc(strlen(filename)+maxsufflen);
	if(tmpfn==NULL)
	{
		AUDDBG("Out of memory\n");
		return NULL;
	}

	strcpy(tmpfn,filename);
	return tmpfn;

}

char * UpdateFileName(char * filename)
{
    char * tmpfilename = NULL;
    AUDDBG("Check existence of file name %s\n",filename);

	if(VFSFile::test_file(filename,VFS_EXISTS) == FALSE)
    {
    	AUDDBG("File doesn't exist. Scan input plugins for available extensions.\n");
        ScanEnxtentions();
        AUDDBG("Check existence of file name %s\n",filename);
//        AUDDBG("Try other suffixes\n");
        tmpfilename=relocatemem(filename);

 /*   	AUDDBG("check for relative paths\n");
    	if(delwindowsrelativepath(tmpfilename))
    	{
    		AUDDBG("wrong path is detected\n");
    		AUDDBG("updated file name %s\n",tmpfilename);
    		if(vfs_file_test(tmpfilename,VFS_EXISTS)==TRUE)
    		{
    	  		AUDDBG("right way way is found\n");
    		}
    		winrelpath=TRUE;
    	}*/
    	// search good suffix
    	int i=0;
    	bool_t exitfl=TRUE;
		//AUDDBG("trying update suffix of file name %s\n",tmpfilename);
    	while(i<suffcnt && exitfl )
    	{
    		AUDDBG("trying update suffix of file name %s\n",tmpfilename);
    		updatesuffix(tmpfilename,i);
    		AUDDBG("trying new file name %s\n",tmpfilename);
    		if(VFSFile::test_file(tmpfilename,VFS_EXISTS) == FALSE)
    		{
    			i++;
    		}else
    		{
    			exitfl=FALSE;
    		}
    	}

    	if(exitfl==FALSE)
    	{
    		AUDDBG("the good file suffix is found %s\n",dsuffixdb[i]);
    		currentsufix=i;
            if(currentsufix!=-1 && filename!=NULL)
            {
  //          	AUDDBG("update filename suffix, suffix before update %s\n",getsuffix(filename));
            	filename=(char*)realloc(filename,strlen(filename)+50);
            	if(filename==NULL)
            	{
            		AUDDBG("Out of memory\n");
            		return FALSE;
            	}

            	updatesuffix(filename,currentsufix);
       //     	AUDDBG("pointer 0x%lX updated %s\n",filename,filename);
      /*      	if(winrelpath==TRUE && filename!=NULL)
    			{
    				AUDDBG("del rel path from %s\n",filename);
    				delwindowsrelativepath(filename);
    			}*/
            }


    	}else
    	{
    		AUDDBG("the good file suffix is not found\n");

    	}

//       	AUDDBG("updated after free(tmpfilename)%s\n",filename);

    }
	free(tmpfilename);
    return filename;
}

typedef struct{
	uint32_t	IndexNum;
	uint32_t	FrameNum;
}CdIndex;



typedef struct {
	char 		*	CueSheet;
	uint64_t		CueSheetLen;

	char		*	CDTitle;
	char		*	CDPerformer;
	char		*	DiskID;
	char		*	Date;
	char		*	Genre;


	char 		**	FileNames;
	char		**	TrackNames;
	char		** 	Performers;
	char		**	Titles;
	CdIndex		*	IndexList;

	uint64_t	*	FileLenghts;

	uint32_t		TitleNumber;
	uint32_t		TrackNumber;
	uint32_t		FileNumber;
	uint32_t		IndexNumber;
	uint32_t		PerformerNumber;
	bool_t			TrackNR;
}CD;

char* CDstrstr(CD * cd,char * str,const char * substr)
{
	if(cd==NULL || str==NULL || substr==NULL || cd==NULL)
	{
		AUDDBG("some of the parameters is NULL\n");
		return NULL;
	}
	uint32_t substrlen=strlen(substr);
	uint32_t cdlen=cd->CueSheetLen;
	char * cdtextend=cd->CueSheet+cdlen-substrlen+1;
	char * i;
	for(i=str;i<cdtextend;i++)
	{
		if(memcmp(i,substr,substrlen)==0) return i;
	}
	return NULL;
}


uint32_t CountStr(CD * cd, char * text,const char * subtext)
{
	if(cd==NULL || text==NULL || subtext==NULL)
	{
		AUDDBG("some of the parameters is NULL\n");
		return 0;
	}
	int cnt=0;
	char *sPtr;
	sPtr=CDstrstr(cd,text,subtext);
	while(sPtr!=NULL)
	{
		cnt++;
		sPtr++;
		sPtr=CDstrstr(cd,sPtr,subtext);
	}
	AUDDBG("found %llu lines %s\n",(LLU_t)cnt,subtext);
	return cnt;
}

char * GetParmStr(CD * cd, char *sPtr,const char * subtext,char ** prms)
{
	if(cd==NULL || sPtr==NULL || prms==NULL || subtext==NULL)
	{
		AUDDBG("some of the parameters is NULL\n");
		return NULL;
	}
	char * rpt;
//	char * ppt;
	char * tsPtr=CDstrstr(cd,sPtr,subtext);
	if(tsPtr!=NULL)
	{
		tsPtr+=strlen(subtext);
		rpt=CDstrstr(cd,tsPtr,"\n");
		if(rpt!=NULL)
			*rpt=0;

		*prms=tsPtr;
		return tsPtr;
	}else
	{
		return NULL;
	}
}

void LoadParmsStr(CD * cd, char * text, char ** list,const char * subtext, uint32_t subnum)
{
	if(cd==NULL || list==NULL || text==NULL || subtext==NULL || subnum==0)
	{
		AUDDBG("some of the parameters is NULL\n");
		return;
	}

	char *sPtr=text;

	for(uint32_t i=0;i<subnum;i++)
		sPtr=GetParmStr(cd,sPtr,subtext,list+i);

	AUDDBG("lines were found:\n");
	for(uint32_t i=0;i<subnum;i++)
	{
		AUDDBG("%s\n",list[i]);
	}
}

char * getkvstr(char* intext)
{
	intext=strstr(intext,"\"")+1;
	if(intext!=NULL)
	{
		char * t=strstr(intext,"\"");
		if(t!=NULL)
		{
			*t=0;
			return intext;
		}
		return NULL;
	}else
		return NULL;
}

void getindxstr(CdIndex *outindex,char * intext)
{
	char * t;
	t=strchr(intext,':');
	if(t==NULL)return;
	*t=' ';
	t=strchr(t,':');
	if(t==NULL)return;
	*t=' ';
	LLU_t q1,q2,q3,q4;
	sscanf(intext,"%llu %llu %llu %llu",&q1,&q2,&q3,&q4);
	outindex->IndexNum=(uint32_t)q1;
	outindex->FrameNum=q4+q3*75+q2*60*75;

}

void deletesuffix(char * text)
{
	char * sfx=getsuffix(text);
	if(sfx>text)
		*(sfx-1)=0;
}



uint32_t CountItems(CD *cd)
{
//	int cnt=0;

	cd->TrackNR = FALSE;

	cd->FileNumber = CountStr(cd,cd->CueSheet,"FILE");
	cd->TrackNumber = CountStr(cd,cd->CueSheet,"TRACK");
	cd->IndexNumber = CountStr(cd,cd->CueSheet,"INDEX");
	cd->TitleNumber = CountStr(cd,cd->CueSheet,"TITLE");
	cd->PerformerNumber = CountStr(cd,cd->CueSheet,"PERFORMER");

	if(cd->FileNumber!=0)
	{
		cd->FileNames = (char **)calloc(cd->FileNumber,sizeof(char*));
		cd->FileLenghts = (uint64_t *)calloc(cd->FileNumber,sizeof(uint64_t));
	}
	if(cd->TrackNumber != 0) cd->TrackNames = (char **)calloc(cd->TrackNumber,sizeof(char*));
	if(cd->IndexNumber != 0) cd->IndexList = (CdIndex *)calloc(cd->IndexNumber,sizeof(CdIndex));
	if(cd->PerformerNumber != 0) cd->Performers = (char **)calloc(cd->PerformerNumber,sizeof(char*));
	if(cd->TitleNumber != 0) cd->Titles = (char **)calloc(cd->TitleNumber,sizeof(char*));

	LoadParmsStr(cd, cd->CueSheet,cd->FileNames,"FILE",cd->FileNumber);
	for(uint32_t i=0;i<cd->FileNumber;i++)
	{
		if(cd->FileNames[i]!=NULL)
		{
			cd->FileNames[i]=getkvstr(cd->FileNames[i]);
			AUDDBG("%s\n",cd->FileNames[i]);
		}
	}

	LoadParmsStr(cd, cd->CueSheet,cd->Performers,"PERFORMER",cd->PerformerNumber);
	for(uint32_t i=0;i<cd->PerformerNumber;i++)
	{
		if(cd->Performers[i]!=NULL)
		{
			cd->Performers[i]=getkvstr(cd->Performers[i]);
			AUDDBG("%s\n",cd->Performers[i]);
		}
	}

	LoadParmsStr(cd, cd->CueSheet,cd->Titles,"TITLE",cd->TitleNumber);
	for(uint32_t i=0;i<cd->TitleNumber;i++)
	{
		cd->Titles[i]=getkvstr(cd->Titles[i]);
		AUDDBG("%s\n",cd->Titles[i]);
	}

	char **tstrl=NULL;
	if(cd->IndexNumber!=0) tstrl=(char **)calloc(cd->IndexNumber,sizeof(char*));
	LoadParmsStr(cd, cd->CueSheet,tstrl,"INDEX",cd->IndexNumber);
	for(uint32_t i=0;i<cd->IndexNumber;i++)
	{
		if(tstrl[i]!=NULL)
		{
			getindxstr(cd->IndexList+i,tstrl[i]);
			AUDDBG("index %llu %llu %llu (%llu mSec)\n",(LLU_t)(i+1), (LLU_t)cd->IndexList[i].IndexNum,(LLU_t)cd->IndexList[i].FrameNum,(LLU_t)(cd->IndexList[i].FrameNum*1000/75));
		}
	}
	free(tstrl);

	if(cd->PerformerNumber==cd->TrackNumber+1)
	{
		cd->CDPerformer=cd->Performers[0];
		for(uint32_t i=0;i<cd->TrackNumber;i++)
			cd->Performers[i]=cd->Performers[i+1];
		cd->PerformerNumber=cd->TrackNumber;
	}else
	{
		if(cd->PerformerNumber==1)
			cd->CDPerformer=cd->Performers[0];
	}


	if(cd->TitleNumber==(cd->TrackNumber+1))
	{
		cd->CDTitle=cd->Titles[0];
		AUDDBG("CD Title: %s\n",cd->CDTitle);

		for(uint32_t i=0;i<cd->TrackNumber;i++)
			cd->Titles[i]=cd->TrackNames[i]=cd->Titles[i+1];
		cd->TitleNumber=cd->TrackNumber;


	}else
	{
		if(cd->TitleNumber==1)
		{
			cd->CDTitle=cd->Titles[0];
			AUDDBG("CD Title: %s\n",cd->CDTitle);
			for(uint32_t i=0;i<cd->TrackNumber;i++)
			{
				cd->TrackNames[i]=(char*)calloc(strlen(cd->FileNames[i])+1,sizeof(char));
				strcpy(cd->TrackNames[i],cd->FileNames[i]);
				deletesuffix(cd->TrackNames[i]);
				AUDDBG("%s\n",cd->TrackNames[i]);
			}
			cd->TrackNR=TRUE;
		}else
		{
			for(uint32_t i=0;i<cd->TitleNumber;i++)
			{
				if(i<cd->TrackNumber)
					cd->TrackNames[i]=cd->Titles[i];
			}
		}
	}
	return 0;
}
void FreeCD(CD *cd)
{
	if(cd->TrackNR==TRUE)
		for(uint32_t i=0;i<cd->TrackNumber;i++)
			free(cd->TrackNames[i]);

	free(cd->FileNames);
	free(cd->FileLenghts);
	free(cd->TrackNames);
	free(cd->IndexList);
	free(cd->Performers);
	free(cd->Titles);
}

uint32_t LoadCue(CD * cd, char* text, uint64_t len)
{
	memset(cd,0,sizeof(CD));
	cd->CueSheet=text;
	cd->CueSheetLen=len;
	CountItems(cd);
	return 0;
}

//static bool_t playlist_load_cue (const char * cue_filename, VFSFile * file,
// char * * title, Index * filenames, Index * tuples)
//static bool_t playlist_load_cue (const char * cue_filename, VFSFile * file,
// String & title, Index<PlaylistAddItem> & items)

bool AutoCueLoader::load (const char * cue_filename, VFSFile & file, String & title,
 Index<PlaylistAddItem> & items)
{
//	index_delete(filenames,0,index_count(filenames));
//	index_delete(tuples,0,index_count(tuples));

    Index<char> bufferpp = file.read_all ();
    if (! bufferpp.len ())
        return false;

    bufferpp.append (0);

    char * buffer = bufferpp.begin ();

    StringBuf text = str_to_utf8 (buffer,bufferpp.len ());

    if (buffer == NULL)
        return FALSE;


    CD cd;
 //   LoadCue (&cd,buffer,size);
    LoadCue (&cd, text, text.len());

#if (1)
    Tuple * tlist;
    char **	flist;

    tlist = new Tuple[cd.FileNumber];

    flist = (char**) malloc(sizeof(char*)*cd.FileNumber);

    for(uint32_t i=0;i<cd.FileNumber;i++)
    {
  //  	tlist[i] = NULL;
    	flist[i] = NULL;
    }

    for(uint32_t i=0;i<cd.FileNumber;i++)
    {
    	AUDDBG("File name %s\n",cd.FileNames[i]);
    	StringBuf tmp = uri_construct (cd.FileNames[i], cue_filename);

    	flist[i] = (char*)malloc(tmp.len()+1);
    	strcpy(flist[i],(char*)tmp);

    	AUDDBG("URL file name %s\n",flist[i]);
    	flist[i] = UpdateFileName(flist[i]);

    	PluginHandle * decoder = aud_file_find_decoder (flist[i], FALSE);

        if (decoder != NULL)
        {
            	AUDDBG("decoder:%s\n",aud_plugin_get_name(decoder));
                tlist[i] = aud_file_read_tuple (flist[i], decoder);
                //if(tlist[i]!=NULL)
                {
                	//cd.FileLenghts[i]=tuple_get_int (tlist[i], FIELD_LENGTH, NULL);
                	cd.FileLenghts[i]=tlist[i].get_int( Tuple::Length );

                	AUDDBG("Track %llu len %llu\n",(LLU_t)i,(LLU_t)cd.FileLenghts[i]);
                }
        }
    }
#endif


    if(cd.TrackNumber==cd.FileNumber)
    {	// Equal number of files and tracks, skip index data, titles and performers
		for(uint32_t i=0;i<cd.FileNumber;i++)
		{
			//if(tlist[i]!=NULL)
			{
				tlist[i].set_int(Tuple::Track, i+1);
				if(cd.TrackNames[i]!=NULL)
					tlist[i].set_str( Tuple::Title, cd.TrackNames[i]);
				if(i < cd.PerformerNumber && cd.Performers[i]!=NULL)
					tlist[i].set_str( Tuple::Artist, cd.Performers[i]);
				if(cd.CDTitle!=NULL)
					tlist[i].set_str( Tuple::Album, cd.CDTitle);

				items.append (String(flist[i]), std::move (tlist[i]));

			}
		}
		goto finish;
    }

    if(cd.FileNumber==1)
    {
    	uint32_t cnt=0;
		for(uint32_t i=0;i<cd.TrackNumber;i++)
		{
			// Search for the first INDEX 01
			while(cd.IndexList[cnt].IndexNum!=1)
			{
				cnt++;
				if(cnt>=cd.IndexNumber)
				{
					cnt=cd.IndexNumber-1;
					break;
				}
			}


			//if(tlist[0]!=NULL)
			{
				Tuple tuple;// = std::move (Tuple(tlist[0]));
				tuple.set_int( Tuple::Track, i+1 );
				if(cd.TrackNames[i]!=NULL)
				tuple.set_str( Tuple::Title, cd.TrackNames[i]);
				if(i < cd.PerformerNumber && cd.Performers[i]!=NULL)
					tuple.set_str( Tuple::Artist, cd.Performers[i]);
				if(cd.CDTitle!=NULL)
					tuple.set_str( Tuple::Album, cd.CDTitle);
				uint64_t begin=cd.IndexList[cnt].FrameNum*1000/75;
				uint64_t end;

				if((cnt+1)<cd.IndexNumber)
					end=cd.IndexList[cnt+1].FrameNum*1000/75;
				else
					end=cd.FileLenghts[0];

				tuple.set_int( Tuple::StartTime, begin);
				tuple.set_int( Tuple::Length, end - begin);
				tuple.set_int( Tuple::EndTime, end);


				items.append (String(flist[0]), std::move (tuple));

			}

			do{
				cnt++;
				if(cnt>=cd.IndexNumber)
				{
					cnt=cd.IndexNumber-1;
					break;
				}

			}while(cd.IndexList[cnt].IndexNum!=1);
		}
		goto finish;
    }

finish:
    for(uint32_t i=0;i<cd.FileNumber;i++)
    		free(flist[i]);

 //   free(tlist);
 //   AUDDBG("tlist deleted\n");
    free(flist);
    AUDDBG("flist deleted\n");
    DelSuffixdb();
    AUDDBG("suf db deleted\n");
    FreeCD(&cd);
    AUDDBG("cd data deleted\n");
    //free(buffer);
    //AUDDBG("cue sheet text deleted\n");
    return TRUE;

}


