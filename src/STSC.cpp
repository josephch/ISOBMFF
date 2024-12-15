/*******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 DigiDNA - www.digidna.net
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/*!
 * @file        STSC.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <ISOBMFF/STSC.hpp>
#include <ISOBMFF/Parser.hpp>
#include <cstdint>
#include <cstring>

namespace ISOBMFF
{
    class STSC::IMPL
    {
        public:

            IMPL();
            IMPL( const IMPL & o );
            ~IMPL();

            std::vector< SampleToChunk > _sample_to_chunk_table;
    };

    STSC::STSC():
        FullBox( "stsc" ),
        impl( std::make_unique< IMPL >() )
    {}

    STSC::STSC( const STSC & o ):
        FullBox( o ),
        impl( std::make_unique< IMPL >( *( o.impl ) ) )
    {}

    STSC::STSC( STSC && o ) noexcept:
        FullBox( std::move( o ) ),
        impl( std::move( o.impl ) )
    {
        o.impl = nullptr;
    }

    STSC::~STSC()
    {}

    STSC & STSC::operator =( STSC o )
    {
        FullBox::operator=( o );
        swap( *( this ), o );

        return *( this );
    }

    void swap( STSC & o1, STSC & o2 )
    {
        using std::swap;

        swap( static_cast< FullBox & >( o1 ), static_cast< FullBox & >( o2 ) );
        swap( o1.impl, o2.impl );
    }

    void STSC::ReadData( Parser & parser, BinaryStream & stream )
    {
        FullBox::ReadData( parser, stream );

        uint32_t entry_count = stream.ReadBigEndianUInt32();

        for( uint32_t i = 0; i < entry_count; i++ )
        {
            this->impl->_sample_to_chunk_table.emplace_back(stream.ReadBigEndianUInt32(), stream.ReadBigEndianUInt32(), stream.ReadBigEndianUInt32());
        }
    }

    std::vector< std::pair< std::string, std::string > > STSC::GetDisplayableProperties() const
    {
        auto props( FullBox::GetDisplayableProperties() );

        for( unsigned int index = 0; index < this->GetEntryCount(); index++ )
        {
            props.push_back( { "Sample To Chunk",  std::to_string( this->GetSampleToChunk(index).firstChunk )  + "." + std::to_string( this->GetSampleToChunk(index).samplesPerChunk) + "." + std::to_string( this->GetSampleToChunk(index).sampleDescriptionId ) } );
        }

        return props;
    }

    size_t STSC::GetEntryCount() const
    {
        return this->impl->_sample_to_chunk_table.size();
    }

    SampleToChunk STSC::GetSampleToChunk( size_t index ) const
    {
        return this->impl->_sample_to_chunk_table[ index ];
    }

    STSC::IMPL::IMPL()
    {}

    STSC::IMPL::IMPL( const IMPL & o )
    {
        this->_sample_to_chunk_table  = o._sample_to_chunk_table;
    }

    STSC::IMPL::~IMPL()
    {}
}
