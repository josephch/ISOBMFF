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
 * @header      STSC.hpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#ifndef ISOBMFF_STSC_HPP
#define ISOBMFF_STSC_HPP

#include <memory>
#include <ISOBMFF/Macros.hpp>
#include <ISOBMFF/FullBox.hpp>
#include <string>

namespace ISOBMFF
{
    struct SampleToChunk
    {
        uint32_t firstChunk;
        uint32_t samplesPerChunk;
        uint32_t sampleDescriptionId;
        
        SampleToChunk(uint32_t firstChunk, uint32_t samplesPerChunk, uint32_t sampleDescriptionId) :  firstChunk(firstChunk), samplesPerChunk(samplesPerChunk), sampleDescriptionId(sampleDescriptionId){}
        
    };

    class ISOBMFF_EXPORT STSC: public FullBox
    {
        public:

            STSC();
            STSC( const STSC & o );
            STSC( STSC && o ) noexcept;
            virtual ~STSC() override;

            STSC & operator =( STSC o );

            void                                                 ReadData( Parser & parser, BinaryStream & stream ) override;
            std::vector< std::pair< std::string, std::string > > GetDisplayableProperties() const override;

            size_t   GetEntryCount()                 const;
            SampleToChunk GetSampleToChunk(  size_t index ) const;

            ISOBMFF_EXPORT friend void swap( STSC & o1, STSC & o2 );

        private:

            class IMPL;

            std::unique_ptr< IMPL > impl;
    };
}

#endif /* ISOBMFF_STSC_HPP */
