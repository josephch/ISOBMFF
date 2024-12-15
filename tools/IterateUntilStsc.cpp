#include <iostream>

#include "ISOBMFF/FTYP.hpp"
#include "ISOBMFF/HDLR.hpp"
#include "ISOBMFF/MVHD.hpp"
#include "ISOBMFF/Parser.hpp"
#include "ISOBMFF/TKHD.hpp"

static void processMinf(ISOBMFF::Container *minf)
{
    std::shared_ptr<ISOBMFF::Box> stblbox = minf->GetBox("stbl");
    if (!stblbox)
    {
        std::cerr << "stblbox not accessible\n";
    }
    else
    {
        ISOBMFF::Container *stbl = dynamic_cast<ISOBMFF::Container *>(stblbox.get());
        if (!stbl)
        {
            std::cerr << "stbl not accessible\n";
        }
        else
        {
            std::shared_ptr<ISOBMFF::Box> stsc = stbl->GetBox("stsc");
            if (!stsc)
            {
                std::cerr << "stsc not accessible\n";
            }
            else
            {
                std::cout << "stsc " << stsc->GetName() << "\n";
                // TODO
            }
        }
    }
}

static void processTrak(ISOBMFF::Container *trak)
{
    std::shared_ptr<ISOBMFF::TKHD> tkhd = trak->GetTypedBox<ISOBMFF::TKHD>("tkhd");
    if (!tkhd)
    {
        std::cerr << "tkhd not accessible\n";
    }
    else
    {
        std::cout << "track id : " << tkhd->GetTrackID() << "\n";
    }
    std::shared_ptr<ISOBMFF::Box> mdiabox = trak->GetBox("mdia");
    if (!mdiabox)
    {
        std::cerr << "mdia not accessible\n";
    }
    else
    {
        ISOBMFF::Container *mdia = dynamic_cast<ISOBMFF::Container *>(mdiabox.get());
        std::shared_ptr<ISOBMFF::HDLR> hdlr = mdia->GetTypedBox<ISOBMFF::HDLR>("hdlr");
        if (!hdlr)
        {
            std::cerr << "hdlr not accessible\n";
        }
        else
        {
            std::cout << "handlerType : " << hdlr->GetHandlerType() << "\n";
        }
        std::shared_ptr<ISOBMFF::Box> minfbox = mdia->GetBox("minf");
        if (!minfbox)
        {
            std::cerr << "minfbox not accessible\n";
        }
        else
        {
            ISOBMFF::Container *minf = dynamic_cast<ISOBMFF::Container *>(minfbox.get());
            if (minf)
            {
                processMinf(minf);
            }
            else
            {
                std::cerr << "minf not accessible\n";
            }
        }
    }
}

bool parseFile(std::string filename)
{
    ISOBMFF::Parser parser;
    try
    {
        parser.Parse(filename);
        std::shared_ptr<ISOBMFF::File> file = parser.GetFile();
        if (!file)
        {
            std::cerr << "File not accessible\n";
            return false;
        }
        std::shared_ptr<ISOBMFF::FTYP> ftyp = file->GetTypedBox<ISOBMFF::FTYP>("ftyp");
        if (!ftyp)
        {
            std::cerr << "ftyp not accessible\n";
            return false;
        }
        std::cout << "ftyp major brand : " << ftyp->GetMajorBrand() << "\n";
        std::shared_ptr<ISOBMFF::ContainerBox> moov = file->GetTypedBox<ISOBMFF::ContainerBox>("moov");
        if (!moov)
        {
            std::cerr << "moov not accessible\n";
            return false;
        }
        std::shared_ptr<ISOBMFF::MVHD> mvhd = moov->GetTypedBox<ISOBMFF::MVHD>("mvhd");
        if (!moov)
        {
            std::cerr << "mvhd not accessible\n";
            return false;
        }
        std::cout << "mvhd duration : " << mvhd->GetDuration() << " scale : " << mvhd->GetTimescale() << "\n";

        std::vector<std::shared_ptr<ISOBMFF::Box> > tracks = moov->ISOBMFF::Container::GetBoxes("trak");
        std::cout << "number of tracks : " << tracks.size() << "\n";
        for (std::shared_ptr<ISOBMFF::Box> item : tracks)
        {
            ISOBMFF::Container *trak = dynamic_cast<ISOBMFF::Container *>(item.get());
            if (trak)
            {
                processTrak(trak);
            }
            else
            {
                std::cerr << "trak not accessible\n";
            }
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage %s <isobmff file path>\n";
        return -1;
    }
    if (!parseFile(argv[1]))
    {
        std::cerr << "Parse failed\nUsage %s <isobmff file path>\n";
        return -1;
    }
    return 0;
}
