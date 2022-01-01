
#include "../bigfx.h"
#include "file.h"

#if 0 //_WIN32 || _WIN64

#else
    #include <dirent.h>
    #include <sys/stat.h>
#endif

namespace bigfx
{

File::File() :
	m_file_handle(nullptr),
	m_async(false)
{


	//m_pRegisterLink = NULL;
}

File::~File()
{
	//SAFE_DELETE( m_pRegisterLink );
	Close();
}

bool File::IsValidHandle()
{
#if 0 //_WIN32 || _WIN64
	return (m_file_handle != nullptr && m_file_handle != INVALID_HANDLE_VALUE);
#else
	return (m_file_handle != nullptr);
#endif
}

bool File::Open( char const* file_name, bool bWriteAccess, bool bAsync )
{
	m_flags |= bWriteAccess ? ArchiveFlag_Write : ArchiveFlag_Read;
	m_flags &= bWriteAccess ? ~ArchiveFlag_Read : ~ArchiveFlag_Write;

	m_async = bAsync;

#if 0 //_WIN32 || _WIN64
	uint32 Flags = FILE_ATTRIBUTE_NORMAL;
	if( bAsync )
		Flags |= FILE_FLAG_OVERLAPPED;

	if( IsReading() )
		m_file_handle = CreateFile( file_name, GENERIC_READ, 0/*FILE_SHARE_READ*/, nullptr, OPEN_EXISTING, Flags, nullptr );
	else
		m_file_handle = CreateFile( file_name, GENERIC_WRITE, 0/*FILE_SHARE_WRITE*/, nullptr, CREATE_ALWAYS, Flags, nullptr );

	if( !m_file_handle || m_file_handle == INVALID_HANDLE_VALUE )
		return false;
#else
	m_file_handle = fopen( file_name, IsReading() ? "rb" : "wb" );
	if( !m_file_handle )
		return false;
#endif

	//if( _bUseLinkTable && _eAccess == eAccessRead )
	//	m_pRegisterLink	= new HashMapType_LT;

	return true;
}

//---------------------------------------------------------------------------------
void File::Close()
{
#if 0 //_WIN32 || _WIN64
	if( m_file_handle && m_file_handle != INVALID_HANDLE_VALUE )
		CloseHandle( m_file_handle );
#else
	if( m_file_handle )
		fclose( m_file_handle );
#endif
	m_file_handle = nullptr;
}

void File::Seek( uint32 Offset )
{
#if 0 //_WIN32 || _WIN64
	// Try to move hFile file pointer some distance  
	DWORD dwPtr = SetFilePointer( m_file_handle, Offset,	NULL, FILE_BEGIN ); 
	if( dwPtr == INVALID_SET_FILE_POINTER )
	{ 
		// Obtain the error code. 
		DWORD dwError = GetLastError() ; 
	}
#else
	fseek( m_file_handle, Offset, SEEK_SET );
#endif
}

uint32 File::Tell()
{
#if 0 //_WIN32 || _WIN64
	// Crappy, but there is no GetFilePointer in Win32 API
	return SetFilePointer( m_file_handle, 0, NULL, FILE_CURRENT );
#else
	return ftell( m_file_handle );
#endif
}

uint32 File::Serialize( void* buffer, uint32 Size )
{
	int SerializedCount = 0;
	if( IsReading() )
	{
#if 0 //_WIN32 || _WIN64
		ReadFile( m_file_handle, buffer, Size, (LPDWORD)&SerializedCount, nullptr );
#else
		SerializedCount = fread( buffer, 1, Size, m_file_handle );
#endif
	}
	else
	{
#if 0 //_WIN32 || _WIN64
		if( !WriteFile( m_file_handle, buffer, Size, (LPDWORD)&SerializedCount, nullptr ) )
		{
			LPVOID lpMsgBuf;
			DWORD dw = GetLastError(); 

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,
				0, NULL );
		}
#else
		SerializedCount = fwrite( buffer, 1, Size, m_file_handle );
#endif
	}
	return SerializedCount;
}

bool File::SerializeAsync( void* buffer, uint32 Size )
{
#if 0 //_WIN32 || _WIN64
	BOOL Result = 1;
	uint32 SerializedCount = 0;
	Memory::Memzero(&m_overlapped, sizeof(m_overlapped));
	if( IsReading() )
	{
		Result = ReadFile(m_file_handle, buffer, Size, (LPDWORD)&SerializedCount, &m_overlapped);
	}
	else
	{
		Result = WriteFile(m_file_handle, buffer, Size, (LPDWORD)&SerializedCount, &m_overlapped);
	}
	if( !Result )
	{
		uint32 ErrorCode = GetLastError();
		if( ErrorCode == ERROR_IO_PENDING )
			return true;

		return false;
	}
	return true;
#else
	return false;
#endif
}

bool File::HasAsyncIOCompleted()
{
#if 0 //_WIN32 || _WIN64
	return HasOverlappedIoCompleted( &m_overlapped );
#else
	return false;
#endif
}

size_t File::GetFileSize()
{
	if( !IsValidHandle() )
		return 0;

	size_t Size = 0;
#if 0 //_WIN32 || _WIN64
	LARGE_INTEGER LInt;
	Memory::Memzero( &LInt, sizeof(LInt) );
	GetFileSizeEx( m_file_handle, &LInt );
	Size = (size_t) LInt.QuadPart;
	//DWORD dwSize = GetFileSize( m_hFile, NULL );
#else
	size_t Pos = ftell(m_file_handle);
	fseek( m_file_handle, 0, SEEK_END );
	Size = ftell(m_file_handle);
	fseek( m_file_handle, Pos, SEEK_SET );
#endif

	return Size;
}
//
//u32	File::fprintf( const char *format, ... )
//{
//	DWORD nToWrite = 0;
//	DWORD nWritten = 0;
//	char buffer[512]; // 512 or the max string you can have... or you can implement memory allocation.
//
//	va_list arguments;
//	va_start(arguments, format);
//
//	vsprintf_s( buffer, sizeof(buffer), format, arguments );
//
//	nToWrite = (DWORD)strlen(buffer);
//	WriteFile( m_hFile, buffer, nToWrite, &nWritten, NULL );
//
//	va_end(arguments); 
//
//	return nWritten;
//
//}

/////////////////////////////////////////////////////////////////////////////////////
namespace FileUtils
{
	bool FileExits(char const* file_name)
	{
#if 0 //_WIN32 || _WIN64
		WIN32_FIND_DATA FileInfo;
		HANDLE FindHandle = ::FindFirstFile(file_name, &FileInfo);
		bool bFileFound = (FindHandle == INVALID_HANDLE_VALUE ? false : true);
		::FindClose(FindHandle);

		return bFileFound;
#else
		if( FILE* file = fopen( file_name, "r" ) ) 
		{
			fclose(file);
			return true;
		} 
		else
			return false;
#endif
	}

	size_t FileSize(char const* file_name)
	{
		size_t Size = 0;
#if 0 //_WIN32 || _WIN64
		HANDLE file_handle = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
			return 0;

		LARGE_INTEGER LInt;
		Memory::Memzero(&LInt, sizeof(LInt));
		GetFileSizeEx(file_handle, &LInt);
		Size = (size_t)LInt.QuadPart;
		CloseHandle(file_handle);
#else
		if( FILE* file = fopen( file_name, "r" ) ) 
		{
			fseek( file, 0, SEEK_SET );
			Size = ftell(file);
			fclose(file);
		} 
#endif

		return Size;
	}

	void ListFiles(char const* strSearch, Array<String>& out_files)
	{
#if 0 //_WIN32 || _WIN64
		WIN32_FIND_DATA file_info;
		HANDLE hFind = FindFirstFile(strSearch, &file_info);
		if (hFind == INVALID_HANDLE_VALUE)
			return;

		out_files.push_back(file_info.cFileName);

		while (FindNextFile(hFind, &file_info))
		{
			out_files.push_back(file_info.cFileName);
		}

		FindClose(hFind);
#else
        DIR* dp = opendir( strSearch );
        if( !dp )
            return;
        
        struct dirent* dirp;
        while ((dirp = readdir(dp)) != nullptr)
        {
            out_files.push_back( dirp->d_name );
        }
        closedir(dp);
#endif
	}
    
    void NormalizePath( String& path )
    {
#if 0 //_WIN32 || _WIN64
		const char* sz_sep = "\\";
		path.Replace('/', '\\', true);
#else
		const char* sz_sep = "/";
        path.Replace( '\\', '/', true );
#endif 
        struct SepVal
        {
            int sep_idx;
            int sep_end; // -1 = . / -2 = .. / > 0 = other (len)
        };
                         
        Array<SepVal> separators;
        int last_idx = path.Len() - 1;
        int sep_idx = 0;
		while ((sep_idx = path.IndexOf(sz_sep, sep_idx)) != INDEX_NONE)
        {
            if( separators.size() )
            {
                if( separators.Last().sep_end >= 0 )
                    separators.Last().sep_end = sep_idx - 1;
            }
            else
                last_idx = sep_idx - 1;
            
            SepVal sep_val;
            sep_val.sep_idx = sep_idx;
            sep_val.sep_end = sep_idx;
            if( sep_idx + 1 < path.Len() && path[sep_idx + 1] == '.' )
            {
                sep_val.sep_end = -1;
                if( sep_idx + 2 < path.Len() && path[sep_idx + 2] == '.' )
                {
                    sep_val.sep_end = -2;
                }
            }
            
            separators.push_back(sep_val);
            sep_idx++;
        }
        
        // simplify path
        sep_idx = 0;
        while( sep_idx < separators.size() )
        {
            if( separators[sep_idx].sep_end == -1 ) // .
            {
                separators.erase(sep_idx);
            }
            else if( separators[sep_idx].sep_end == -2 ) // ..
            {
                if( sep_idx > 0 && separators[sep_idx - 1].sep_end >= 0 )
                {
                    sep_idx--;
                    separators.erase(sep_idx);
                    separators.erase(sep_idx);
                }
                else
                    sep_idx++;
            }
            else
                sep_idx++;
        }
                    
        String out_path = "";
        out_path = path.Sub(0, last_idx + 1);
        sep_idx = 0;
        while( sep_idx < separators.size() )
        {
            SepVal& sep_val = separators[sep_idx];
            if( sep_val.sep_end == -1 ) // .
            {
				out_path += sz_sep;
				out_path += ".";
            }
            else if( sep_val.sep_end == -2 ) // ..
            {
				out_path += sz_sep;
				out_path += "..";
            }
            else
            {
                out_path += path.Sub(sep_val.sep_idx, sep_val.sep_end - sep_val.sep_idx + 1);
            }
            sep_idx++;
        }
        path = out_path;
    }

	bool IsDirectory(char const* filename)
	{
#if 0 //_WIN32 || _WIN64
		DWORD file_attr = GetFileAttributes(filename);
		if (file_attr == INVALID_FILE_ATTRIBUTES)
			return false;

		if (file_attr & FILE_ATTRIBUTE_DIRECTORY)
			return true;
        
        return false;
#else
        struct stat st_buf;
        int status = stat (filename, &st_buf);
        if (status == 0)
        {
            if (S_ISDIR (st_buf.st_mode))
                return true;
                
            //S_ISREG (st_buf.st_mode);
        }
        
		return false;
#endif
	}
} // namespace FileUtils



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

    
uint32 Archive::SerializeString( String& BufferStr )
{
    uint32 serialized_count = 0;
    if( IsReading() )
    {
        BufferStr = "";
        char Buffer[256];
        uint32 SizeRead = 0;
        
        while( (SizeRead = Serialize( Buffer, sizeof(Buffer) )) > 0 )
        {
            int OldLen = BufferStr.Len();
            BufferStr.resize( OldLen + SizeRead );
            Memory::Memcpy( &BufferStr[OldLen], Buffer, SizeRead );
        }
		serialized_count = BufferStr.Len();
    }
    else
    {
		serialized_count = Serialize(BufferStr.c_str(), BufferStr.Len());
    }
    
	return serialized_count;
}

uint32 Archive::WriteString(char const* str)
{
	uint32 serialized_count = 0;
	
	if (IsWriting())
		serialized_count = Serialize((void*)str, (uint32)strlen(str));

	return serialized_count;
}
    
uint32 MemoryReader::Serialize( void* buffer, uint32 size )
{
	// Ensure we have enough data
	if (m_offset + size <= (uint32)m_data.size())
	{
		Memory::Memcpy(buffer, &m_data[m_offset], size);
		m_offset += size;
		return size;
	}

	return 0;
}


uint32 MemoryWriter::Serialize(void* buffer, uint32 size)
{
	uint32 size_needed = m_offset + size;
	if (size_needed > (uint32)m_data.size())
	{
		m_data.resize(size_needed);
	}
	if (size)
	{
		Memory::Memcpy(&m_data[m_offset], buffer, size);
		m_offset += size;
	}
	return size;
}

} /* namespace bigfx */
