#include <Grid/Grid.h>

using namespace Grid;

int main (int argc, char **argv)
{
    Grid_init(&argc,&argv);
    
    Coordinate latt_size   = GridDefaultLatt();
    Coordinate simd_layout = GridDefaultSimd(Nd,vComplex::Nsimd());
    Coordinate mpi_layout  = GridDefaultMpi();

    GridCartesian             Grid(latt_size,simd_layout,mpi_layout);
    GridRedBlackCartesian     RBGrid(&Grid);
    
    LatticeGaugeField Umu(&Grid);
    LatticeGaugeField identity(&Grid);
    LatticeColourMatrixD U(&Grid);
    LatticeColourMatrixD Cidentity(&Grid);
    LatticeColourMatrixD aux(&Grid);
    
    identity = 1.0;
    Cidentity = 1.0;
    Complex i(0., 1.);
    
    double vol = Umu.Grid()->gSites();
    
    const int nsp = Nc / 2;
    std::vector<int> pseeds({1,2,3,4,5});
    std::vector<int> sseeds({6,7,8,9,10});
    GridParallelRNG  pRNG(&Grid); pRNG.SeedFixedIntegers(pseeds);
    GridSerialRNG    sRNG;       sRNG.SeedFixedIntegers(sseeds);
    
    std::cout << GridLogMessage << std::endl;
    
    
    Sp<nsp>::ColdConfiguration(pRNG,Umu);
    Umu = Umu - identity;
    U = PeekIndex<LorentzIndex>(Umu,1);
    
    std::cout << GridLogMessage << "U - 1 = " << norm2(sum(U)) << std::endl;
    assert ( norm2(sum(U)) < 1e-6);
    std::cout << GridLogMessage << std::endl;
    
    std::cout << GridLogMessage << "Hot Start " << std::endl;
    Sp<nsp>::HotConfiguration(pRNG,Umu);
    U = PeekIndex<LorentzIndex>(Umu,1);
    
    std::cout << GridLogMessage << "Checking unitarity " << std::endl;
    
    aux = U*adj(U) - Cidentity;

    assert( norm2(aux) < 1e-6 );
    
    std::cout << GridLogMessage << "ok" << std::endl;
    std::cout << GridLogMessage << std::endl;
    
    std::cout << GridLogMessage << "Checking determinant " << std::endl;
    std::cout << GridLogMessage << "Det = " <<  norm2( Determinant(U) )/vol << std::endl;
    std::cout << GridLogMessage << std::endl;
    
    std::cout << GridLogMessage << "Checking the matrix is in Sp2n " << std::endl;
    std::cout << GridLogMessage << std::endl;
    
    for (int c1 = 0; c1 < nsp; c1++) //check on W
    {
        for (int c2 = 0; c2 < nsp; c2++)
        {
            auto W = PeekIndex<ColourIndex>(U,c1,c2);
            auto Wstar =  PeekIndex<ColourIndex>(U,c1+nsp,c2+nsp);
            auto Ww = conjugate( Wstar );
            auto amizero = sum(W - Ww);
            auto amizeroo = TensorRemove(amizero);
            std::cout << GridLogMessage << "diff(real,im) = " << amizeroo << std::endl;
            assert(  amizeroo.real() < 10e-6 );
            amizeroo *= i;
            assert(  amizeroo.real() < 10e-6 );
            std::cout << GridLogMessage << "ok " << std::endl;
        }
        
    }
    std::cout <<GridLogMessage << std::endl;
    std::cout << GridLogMessage << "Checking that U [ i , j+N/2 ] + conj U [ i+N/2, j+N/2 ] = 0 for i = 0, ... , N/2 " << std::endl;
    std::cout <<GridLogMessage << std::endl;
    for (int c1 = 0; c1 < nsp ; c1++)
    {
        for (int c2 = 0; c2 < nsp; c2++)
        {
            auto X = PeekIndex<ColourIndex>(U,c1,c2+nsp);
            auto minusXstar = PeekIndex<ColourIndex>(U,c1+nsp,c2);
            auto minusXx = conjugate(minusXstar);
            auto amizero = sum (X + minusXx);
            auto amizeroo = TensorRemove(amizero);
            std::cout << GridLogMessage << "diff(real,im) = " << amizeroo << std::endl;
            assert(  amizeroo.real() < 10e-6 );
            amizeroo *= i;
            assert(  amizeroo.real() < 10e-6 );
            std::cout << GridLogMessage << "ok " << std::endl;
        }
    }
    
    
    std::vector<int> ppseeds({11,12,13,14,15});
    std::vector<int> ssseeds({16,17,18,19,20});
    GridParallelRNG  ppRNG(&Grid); pRNG.SeedFixedIntegers(ppseeds);
    GridSerialRNG    ssRNG;       sRNG.SeedFixedIntegers(ssseeds);
    
    std::cout << GridLogMessage << std::endl;
    std::cout << GridLogMessage << "Testing gauge implementation " << std::endl;
    
    SymplPeriodicGimplR::HotConfiguration(ppRNG, Umu);
    
    U = PeekIndex<LorentzIndex>(Umu,1);
    
    for (int c1 = 0; c1 < nsp; c1++) //check on W
    {
        for (int c2 = 0; c2 < nsp; c2++)
        {
            auto W = PeekIndex<ColourIndex>(U,c1,c2);
            auto Wstar =  PeekIndex<ColourIndex>(U,c1+nsp,c2+nsp);
            auto Ww = conjugate( Wstar );
            auto amizero = sum(W - Ww);
            auto amizeroo = TensorRemove(amizero);
            std::cout << GridLogMessage << "diff(real,im) = " << amizeroo << std::endl;
            assert(  amizeroo.real() < 10e-6 );
            amizeroo *= i;
            assert(  amizeroo.real() < 10e-6 );
            std::cout << GridLogMessage << "ok " << std::endl;
        }
        
    }
    std::cout <<GridLogMessage << std::endl;
    std::cout << GridLogMessage << "Checking that U [ i , j+N/2 ] + conj U [ i+N/2, j+N/2 ] = 0 for i = 0, ... , N/2 " << std::endl;
    std::cout <<GridLogMessage << std::endl;
    for (int c1 = 0; c1 < nsp ; c1++)
    {
        for (int c2 = 0; c2 < nsp; c2++)
        {
            auto X = PeekIndex<ColourIndex>(U,c1,c2+nsp);
            auto minusXstar = PeekIndex<ColourIndex>(U,c1+nsp,c2);
            auto minusXx = conjugate(minusXstar);
            auto amizero = sum (X + minusXx);
            auto amizeroo = TensorRemove(amizero);
            std::cout << GridLogMessage << "diff(real,im) = " << amizeroo << std::endl;
            assert(  amizeroo.real() < 10e-6 );
            amizeroo *= i;
            assert(  amizeroo.real() < 10e-6 );
            std::cout << GridLogMessage << "ok " << std::endl;
        }
    }
    
    
    
    Grid_finalize();


    }

