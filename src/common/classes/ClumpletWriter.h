/*
 *	PROGRAM:	Client/Server Common Code
 *	MODULE:		ClumpletWriter.h
 *	DESCRIPTION:	Secure handling of clumplet buffers
 *
 *  The contents of this file are subject to the Initial
 *  Developer's Public License Version 1.0 (the "License");
 *  you may not use this file except in compliance with the
 *  License. You may obtain a copy of the License at
 *  http://www.ibphoenix.com/main.nfs?a=ibphoenix&page=ibp_idpl.
 *
 *  Software distributed under the License is distributed AS IS,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied.
 *  See the License for the specific language governing rights
 *  and limitations under the License.
 *
 *  The Original Code was created by Nickolay Samofatov
 *  for the Firebird Open Source RDBMS project.
 *
 *  Copyright (c) 2004 Nickolay Samofatov <nickolay@broadviewsoftware.com>
 *  and all contributors signed below.
 *
 *  All Rights Reserved.
 *  Contributor(s): ______________________________________.
 *
 *
 */

#ifndef CLUMPLETWRITER_H
#define CLUMPLETWRITER_H

#include "../common/classes/ClumpletReader.h"
#include "../common/classes/array.h"

// This setting of maximum dpb size doesn't mean, that we
// can't process larger DBPs! This is just recommended limit
// cause it's hard to imagine a useful DPB of even this size.
inline constexpr FB_SIZE_T MAX_DPB_SIZE = 1024 * 1024;

namespace Firebird {

// At the moment you can only declare it on stack, permanent objects are not allowed
class ClumpletWriter : public ClumpletReader
{
public:
	// Create empty clumplet writer.
	ClumpletWriter(Kind k, FB_SIZE_T limit, UCHAR tag = 0);
	ClumpletWriter(MemoryPool& pool, Kind k, FB_SIZE_T limit, UCHAR tag = 0);

	// Create writer from a given buffer
	ClumpletWriter(Kind k, FB_SIZE_T limit, const UCHAR* buffer, FB_SIZE_T buffLen, UCHAR tag = 0);
	ClumpletWriter(MemoryPool& pool, Kind k, FB_SIZE_T limit, const UCHAR* buffer, FB_SIZE_T buffLen, UCHAR tag = 0);

	// Create writer from a given buffer with possibly different clumplet version
	ClumpletWriter(const KindList* kl, FB_SIZE_T limit, const UCHAR* buffer, FB_SIZE_T buffLen);
	ClumpletWriter(MemoryPool& pool, const KindList* kl, FB_SIZE_T limit,
				   const UCHAR* buffer, FB_SIZE_T buffLen);

	// Create empty writer
	ClumpletWriter(const KindList* kl, FB_SIZE_T limit);
	ClumpletWriter(MemoryPool& pool, const KindList* kl, FB_SIZE_T limit);

	// Create a copy of writer
	ClumpletWriter(MemoryPool& pool, const ClumpletWriter& from);
	ClumpletWriter(const ClumpletWriter& from);

	void reset(UCHAR tag = 0);
	void reset(const UCHAR* buffer, const FB_SIZE_T buffLen);
	void reset(const ClumpletWriter& from);
	void clear();

	// Methods to create new clumplet at current position
	void insertInt(UCHAR tag, const SLONG value);
	void insertBigInt(UCHAR tag, const SINT64 value);
	void insertBytes(UCHAR tag, const void* bytes, FB_SIZE_T length);
	void insertString(UCHAR tag, const char* str, FB_SIZE_T length);
	void insertString(UCHAR tag, const char* str);
	void insertString(UCHAR tag, char* str);
	void insertData(UCHAR tag, const UCharBuffer& data);
	void insertByte(UCHAR tag, const UCHAR byte);
	void insertTag(UCHAR tag);
	void insertDouble(UCHAR tag, const double value);
	void insertTimeStamp(UCHAR tag, const ISC_TIMESTAMP value);
	void insertTime(UCHAR tag, ISC_TIME value) { insertInt(tag, value); }
	void insertDate(UCHAR tag, ISC_DATE value) { insertInt(tag, value); }
	void insertEndMarker(UCHAR tag);
	void insertClumplet(const SingleClumplet& clumplet);

	template <typename S>
	void insertString(UCHAR tag, const S& str)
	{
		insertString(tag, str.c_str(), str.length());
	}

    // Delete currently selected clumplet from buffer
	void deleteClumplet();

	// Delete all clumplets with given tag
	// Returns true if any found
	bool deleteWithTag(UCHAR tag);

	const UCHAR* getBuffer() const override;
	bool hasOverflow() const noexcept
	{
		return flag_overflow;
	}

protected:
	const UCHAR* getBufferEnd() const override;
	virtual void size_overflow();
	void insertBytesLengthCheck(UCHAR tag, const void* bytes, const FB_SIZE_T length);
	bool upgradeVersion();	// upgrade clumplet version - obtain newest from kindList

private:
	// Assignment not implemented.
	ClumpletWriter& operator=(const ClumpletWriter& from);

	FB_SIZE_T sizeLimit;
	const KindList* kindList;
	HalfStaticArray<UCHAR, 128> dynamic_buffer;

	void initNewBuffer(UCHAR tag);
	void create(const UCHAR* buffer, FB_SIZE_T buffLen, UCHAR tag);
	static void toVaxInteger(UCHAR* ptr, FB_SIZE_T length, const SINT64 value);

	void size_overflow(bool condition);
	bool flag_overflow;
};

} // namespace Firebird

#endif // CLUMPLETWRITER_H
