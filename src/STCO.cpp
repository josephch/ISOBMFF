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
 * @file        STCO.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <STCO.hpp>
#include <Parser.hpp>
#include <cstdint>
#include <cstring>

namespace ISOBMFF
{
    class STCO::IMPL
    {
        public:

            IMPL();
            IMPL( const IMPL & o );
            ~IMPL();

            std::vector< uint32_t > _chunk_offset_table;
    };

    STCO::STCO():
        FullBox( "stco" ),
        impl( std::make_unique< IMPL >() )
    {}

    STCO::STCO( const STCO & o ):
        FullBox( o ),
        impl( std::make_unique< IMPL >( *( o.impl ) ) )
    {}

    STCO::STCO( STCO && o ) noexcept:
        FullBox( std::move( o ) ),
        impl( std::move( o.impl ) )
    {
        o.impl = nullptr;
    }

    STCO::~STCO()
    {}

    STCO & STCO::operator =( STCO o )
    {
        FullBox::operator=( o );
        swap( *( this ), o );

        return *( this );
    }

    void swap( STCO & o1, STCO & o2 )
    {
        using std::swap;

        swap( static_cast< FullBox & >( o1 ), static_cast< FullBox & >( o2 ) );
        swap( o1.impl, o2.impl );
    }

    Error STCO::ReadData( Parser & parser, BinaryStream & stream )
    {
        FullBox::ReadData( parser, stream );

        uint32_t entry_count;
        Error err = stream.ReadBigEndianUInt32(entry_count);
        if (err) return err;

        for( uint32_t i = 0; i < entry_count; i++ )
        {
            uint32_t chunkOffset;
            err = stream.ReadBigEndianUInt32(chunkOffset);
            if (err) return err;
            this->impl->_chunk_offset_table.emplace_back(chunkOffset);
        }
        return err;
    }

    std::vector< std::pair< std::string, std::string > > STCO::GetDisplayableProperties() const
    {
        auto props( FullBox::GetDisplayableProperties() );

        for( unsigned int index = 0; index < this->GetEntryCount(); index++ )
        {
            props.push_back( { "Sample To Chunk",  std::to_string( this->GetChunkOffset(index) ) } );
        }

        return props;
    }

    size_t STCO::GetEntryCount() const
    {
        return this->impl->_chunk_offset_table.size();
    }

    uint32_t STCO::GetChunkOffset( size_t index ) const
    {
        return this->impl->_chunk_offset_table[ index ];
    }

    STCO::IMPL::IMPL()
    {}

    STCO::IMPL::IMPL( const IMPL & o )
    {
        this->_chunk_offset_table  = o._chunk_offset_table;
    }

    STCO::IMPL::~IMPL()
    {}
}
