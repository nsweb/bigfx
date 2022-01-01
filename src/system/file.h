
#ifndef BB_SYSTEMFILE_H
#define BB_SYSTEMFILE_H 


namespace bigfx
{

#define ArchiveFlag_Read		(0x00000001)
#define ArchiveFlag_Write		(0x00000002)
#define ArchiveFlag_Compressed  (0x00000004)


/////////////////////////////////////////////////////////////////////
class Archive
{
public:
						Archive() : m_flags(0), m_version(0) {}
						~Archive() {}

	template<typename K>
	uint32				SerializeRaw( K& val )			{ return Serialize( &val, sizeof(K) );		}
	virtual uint32		Serialize( void* buffer, uint32 size ) = 0;
    uint32				SerializeString( String& buffer_str );
	uint32				WriteString(char const* str);
	virtual	void		SerializeVersion(uint32& version) { Serialize(&version, sizeof(version)); m_version = version; }
	uint32				IsReading()		{ return m_flags & ArchiveFlag_Read; }
	uint32				IsWriting() 	{ return m_flags & ArchiveFlag_Write; }
	virtual void		Seek( uint32 offset ) = 0;
	virtual uint32		Tell() = 0;
	inline uint32		GetVersion() { return m_version; }

	uint32			m_flags;
	uint32			m_version;
};

/////////////////////////////////////////////////////////////////////
class MemoryArchive : public Archive
{
public:
	MemoryArchive() : m_offset(0)		{}
	~MemoryArchive()					{}

	void	ResizeData(uint32 size)	{ m_data.resize(size); }
	uint8*	Data()
	{ 
		BB_ASSERT(m_data.size() > 0);
		return m_data.Data();
	}
	virtual void		Seek( uint32 offset )	
	{ 
		BB_ASSERT(offset <= (uint32)m_data.size());
		m_offset = offset;
	}
	virtual uint32		Tell()			{ return m_offset; }
	uint32	Size()						{ return (uint32)m_data.size(); }

protected:
	Array<uint8>		m_data;
	uint32				m_offset;
};

/////////////////////////////////////////////////////////////////////
class MemoryReader : public MemoryArchive
{
public:
	MemoryReader()						{ m_flags |= ArchiveFlag_Read; }
	~MemoryReader()						{}

	virtual uint32		Serialize( void* buffer, uint32 size );
};

/////////////////////////////////////////////////////////////////////
class MemoryWriter : public MemoryArchive
{
public:
	MemoryWriter()						{ m_flags |= ArchiveFlag_Write; }
	~MemoryWriter()						{}

	virtual uint32		Serialize(void* buffer, uint32 size);
};

/////////////////////////////////////////////////////////////////////
class File : public Archive
{
public:
						File();
						~File();

	bool				Open( char const* filename, bool write_access, bool async = false );
	void				Close();
	size_t				GetFileSize();
	bool				IsValidHandle();
	bool				HasAsyncIOCompleted();

	virtual uint32		Serialize(void* buffer, uint32 size);
	bool				SerializeAsync(void* buffer, uint32 size);
	virtual void		Seek( uint32 offset );
	virtual uint32		Tell();

private:
	bool			m_async;

#if 0// _WIN32 || _WIN64
	HANDLE			m_file_handle;
	OVERLAPPED		m_overlapped;
#else
	FILE*			m_file_handle;
#endif
};

/////////////////////////////////////////////////////////////////////
namespace FileUtils
{
	BIGFX_API bool FileExits(char const* filename);
	BIGFX_API size_t FileSize(char const* filename);
	BIGFX_API void ListFiles(char const* str_search, Array<String>& out_files);
    BIGFX_API void NormalizePath( String& path );
	BIGFX_API bool IsDirectory(char const* filename);
};

#if _WIN32 || _WIN64
#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif

struct TgaFileHeader
{
	uint8	identsize;          // size of ID field that follows 18 byte header (0 usually)
	uint8	colourmaptype;      // type of colour map 0=none, 1=has palette
	uint8	imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	int16	colourmapstart;     // first colour map entry in palette
	int16	colourmaplength;    // number of colours in palette
	uint8	colourmapbits;      // number of bits per palette entry 15,16,24,32

	int16	xstart;             // image x origin
	int16	ystart;             // image y origin
	int16	width;              // image width in pixels
	int16	height;             // image height in pixels
	uint8	bits;               // image bits per pixel 8,16,24,32
	uint8	descriptor;         // image descriptor bits (vh flip bits)

	// pixel data follows header
};

struct BGRColor
{
	uint8 b;
	union
	{
		struct 
		{
			uint8 g;
			uint8 r;
		};
		uint16 gr;
	};
};
struct RGBColor
{
	uint8 r;
	union
	{
		struct 
		{
			uint8 g;
			uint8 b;
		};
		uint16 gb;
	};
};

#if _WIN32 || _WIN64
#pragma pack(pop)
#endif



}  /* namespace bigfx */


#endif // BB_SYSTEMFILE_H