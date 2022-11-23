////////////////////////////////////////////////////////////////////////
//
// * Two index representation generators
//
// * Normalisation for the fundamental generators:
//   trace ta tb = 1/2 delta_ab = T_F delta_ab
//   T_F = 1/2  for SU(N) groups
//
//
//   base for NxN two index (anti-symmetric) matrices
//   normalized to 1 (d_ij is the kroenecker delta)
//
//   (e^(ij)_{kl} = 1 / sqrt(2) (d_ik d_jl +/- d_jk d_il)
//
//   Then the generators are written as
//
//   (iT_a)^(ij)(lk) = i * ( tr[e^(ij)^dag e^(lk) T^trasp_a] +
//   tr[e^(lk)e^(ij)^dag T_a] )  //
//   
//
////////////////////////////////////////////////////////////////////////

// Authors: David Preti, Guido Cossu

#ifndef QCD_UTIL_SP2N2INDEX_H
#define QCD_UTIL_SP2N2INDEX_H


NAMESPACE_BEGIN(Grid);

//enum SpTwoIndexSymmetry { S = 1, AS = -1 };

//inline Real delta(int a, int b) { return (a == b) ? 1.0 : 0.0; }

template <int ncolour, TwoIndexSymmetry S>
class Sp_TwoIndex : public Sp<Config_nsp> {
public:
  static const int nsp = ncolour/2;
  static const int Dimension = nsp * (ncolour + S) - 1 ;
  static const int NumGenerators = Sp<nsp>::AlgebraDimension;

  template <typename vtype>
  using iSp2nTwoIndexMatrix = iScalar<iScalar<iMatrix<vtype, Dimension> > >;

  typedef iSp2nTwoIndexMatrix<Complex> TIMatrix;
  typedef iSp2nTwoIndexMatrix<ComplexF> TIMatrixF;
  typedef iSp2nTwoIndexMatrix<ComplexD> TIMatrixD;

  typedef iSp2nTwoIndexMatrix<vComplex> vTIMatrix;
  typedef iSp2nTwoIndexMatrix<vComplexF> vTIMatrixF;
  typedef iSp2nTwoIndexMatrix<vComplexD> vTIMatrixD;

  typedef Lattice<vTIMatrix> LatticeTwoIndexMatrix;
  typedef Lattice<vTIMatrixF> LatticeTwoIndexMatrixF;
  typedef Lattice<vTIMatrixD> LatticeTwoIndexMatrixD;

  typedef Lattice<iVector<iScalar<iMatrix<vComplex, Dimension> >, Nd> >
  LatticeTwoIndexField;
  typedef Lattice<iVector<iScalar<iMatrix<vComplexF, Dimension> >, Nd> >
  LatticeTwoIndexFieldF;
  typedef Lattice<iVector<iScalar<iMatrix<vComplexD, Dimension> >, Nd> >
  LatticeTwoIndexFieldD;

  template <typename vtype>
  using iSp2nMatrix = iScalar<iScalar<iMatrix<vtype, ncolour> > >;

  typedef iSp2nMatrix<Complex> Matrix;
  typedef iSp2nMatrix<ComplexF> MatrixF;
  typedef iSp2nMatrix<ComplexD> MatrixD;

  template <class cplx>
  static void base(int Index, iSp2nMatrix<cplx> &eij) {
    // returns (e)^(ij)_{kl} necessary for change of base U_F -> U_R
    assert(Index < NumGenerators);
    eij = Zero();

    // for the linearisation of the 2 indexes 
    static int a[ncolour * (ncolour - 1) / 2][2]; // store the a <-> i,j
    static bool filled = false;
    if (!filled) {
      int counter = 0;
      for (int i = 1; i < ncolour; i++) {
        for (int j = 0; j < i; j++) {
           
            if (i==nsp)
            {
                j = j+1;
            }
          a[counter][0] = i;
          a[counter][1] = j;
          counter++;
        }
      }
      filled = true;
    }

    if (Index < ncolour * (ncolour - 1) / 2) {
      baseOffDiagonal(a[Index][0], a[Index][1], eij);
    } else {
      baseDiagonal(Index, eij);
    }
  }
    


  template <class cplx>
  static void baseDiagonal(int Index, iSp2nMatrix<cplx> &eij) {
    eij = Zero();
    eij()()(Index - ncolour * (ncolour - 1) / 2,
            Index - ncolour * (ncolour - 1) / 2) = 1.0;
  }

  template <class cplx>
  static void baseOffDiagonal(int i, int j, iSp2nMatrix<cplx> &eij) {
    eij = Zero();
    //std::cout << GridLogMessage << " doing i j = " << i << j << std::endl;
      RealD tmp;

    if ( (i == nsp + j) && (1 <= j) && (j < nsp) )
        {
            for (int k = 0; k < nsp; k++)
            {
                if (k < j)
                {
                    //std::cout << GridLogMessage << "b=N+a 1"<< std::endl;
                    //std::cout << GridLogMessage << "j i "<< j << i << std::endl;
                    tmp = sqrt( 2*j*(j+1)  );
                    tmp = 1/tmp;
                    tmp *= std::sqrt(2.0);
                    eij()()(k,k+nsp) =  tmp;
                    eij()()(k+nsp,k) =  - tmp;
                }
                if (k == j)
                {
                    //std::cout << GridLogMessage << "b=N+a 2"<< std::endl;
                    //std::cout << GridLogMessage << "j i "<< j << i << std::endl;
                    tmp = sqrt(2*j*(j+1)  );
                    tmp = -j/tmp;
                    tmp *= std::sqrt(2.0);
                    eij()()(k,k+nsp) = tmp ;
                    eij()()(k+nsp,k) = - tmp ;
                 }
            }
            
        }
      
      
        else if (i != nsp+j)
        {
            for (int k = 0; k < ncolour; k++)
                for (int l = 0; l < ncolour; l++)
                {
                    eij()()(l, k) = delta(i, k) * delta(j, l) + S * delta(j, k) * delta(i, l);
                }
            
        }
      
          
    
        

    RealD nrm = 1. / std::sqrt(2.0);
    eij = eij * nrm;
  }

  static void printBase(void) {
    for (int gen = 0; gen < Dimension; gen++) {
      Matrix tmp;
      base(gen, tmp);
      std::cout << GridLogMessage << "Nc = " << ncolour << " t_" << gen
                << std::endl;
      std::cout << GridLogMessage << tmp << std::endl;
    }
  }

  template <class cplx>
  static void generator(int Index, iSp2nTwoIndexMatrix<cplx> &i2indTa) {
    Vector<typename Sp<nsp>::template iSp2nMatrix<cplx> > ta(
								NumGenerators);
    Vector<typename Sp<nsp>::template iSp2nMatrix<cplx> > eij(Dimension);
    typename Sp<nsp>::template iSp2nMatrix<cplx> tmp;
    i2indTa = Zero();
    
    for (int a = 0; a < NumGenerators; a++)
      Sp<nsp>::generator(a, ta[a]);
    
    for (int a = 0; a < Dimension; a++) base(a, eij[a]);

    for (int a = 0; a < Dimension; a++) {
      tmp = transpose(ta[Index]) * adj(eij[a]) + adj(eij[a]) * ta[Index];
      for (int b = 0; b < Dimension; b++) {
        typename Sp<nsp>::template iSp2nMatrix<cplx> tmp1 =
	  tmp * eij[b]; 
        Complex iTr = TensorRemove(timesI(trace(tmp1)));
        i2indTa()()(a, b) = iTr;
      }
    }
  }

  static void printGenerators(void) {
    for (int gen = 0; gen < NumGenerators; gen++) {
      TIMatrix i2indTa;
      generator(gen, i2indTa);
      std::cout << GridLogMessage << "Nc = " << ncolour << " t_" << gen
                << std::endl;
      std::cout << GridLogMessage << i2indTa << std::endl;
    }
  }

  static void testGenerators(void) {
    TIMatrix i2indTa, i2indTb;
    std::cout << GridLogMessage << "2IndexRep - Checking if traceless"
              << std::endl;
    for (int a = 0; a < NumGenerators; a++) {
      generator(a, i2indTa);
      std::cout << GridLogMessage << a << std::endl;
      assert(norm2(trace(i2indTa)) < 1.0e-6);
    }
    std::cout << GridLogMessage << std::endl;

    std::cout << GridLogMessage << "2IndexRep - Checking if antihermitean"
              << std::endl;
    for (int a = 0; a < NumGenerators; a++) {
      generator(a, i2indTa);
      std::cout << GridLogMessage << a << std::endl;
      assert(norm2(adj(i2indTa) + i2indTa) < 1.0e-6);
    }

    std::cout << GridLogMessage << std::endl;
    std::cout << GridLogMessage
              << "2IndexRep - Checking Tr[Ta*Tb]=delta(a,b)*(N +- 2)/2"
              << std::endl;
    for (int a = 0; a < NumGenerators; a++) {
      for (int b = 0; b < NumGenerators; b++) {
        generator(a, i2indTa);
        generator(b, i2indTb);

        // generator returns iTa, so we need a minus sign here
        Complex Tr = -TensorRemove(trace(i2indTa * i2indTb));
        std::cout << GridLogMessage << "a=" << a << "b=" << b << "Tr=" << Tr
                  << std::endl;
      }
    }
    std::cout << GridLogMessage << std::endl;
  }

  static void TwoIndexLieAlgebraMatrix(
				       const typename Sp<nsp>::LatticeAlgebraVector &h,
				       LatticeTwoIndexMatrix &out, Real scale = 1.0) {
    conformable(h, out);
    GridBase *grid = out.Grid();
    LatticeTwoIndexMatrix la(grid);
    TIMatrix i2indTa;

    out = Zero();
    for (int a = 0; a < NumGenerators; a++) {
      generator(a, i2indTa);
      la = peekColour(h, a) * i2indTa;
      out += la;
    }
    out *= scale;
  }

  // Projects the algebra components 
  // of a lattice matrix ( of dimension ncol*ncol -1 )
  static void projectOnAlgebra(
			       typename Sp<nsp>::LatticeAlgebraVector &h_out,
			       const LatticeTwoIndexMatrix &in, Real scale = 1.0) {
    conformable(h_out, in);
    h_out = Zero();
    TIMatrix i2indTa;
    Real coefficient = -2.0 / (ncolour + 2 * S) * scale;
    // 2/(Nc +/- 2) for the normalization of the trace in the two index rep
    for (int a = 0; a < NumGenerators; a++) {
      generator(a, i2indTa);
      auto tmp = real(trace(i2indTa * in)) * coefficient;
      pokeColour(h_out, tmp, a);
    }
  }

  // a projector that keeps the generators stored to avoid the overhead of
  // recomputing them
  static void projector(typename Sp<nsp>::LatticeAlgebraVector &h_out,
                        const LatticeTwoIndexMatrix &in, Real scale = 1.0) {
    conformable(h_out, in);
    // to store the generators
    static std::vector<TIMatrix> i2indTa(NumGenerators);
    h_out = Zero();
    static bool precalculated = false;
    if (!precalculated) {
      precalculated = true;
      for (int a = 0; a < NumGenerators; a++) generator(a, i2indTa[a]);
    }

    Real coefficient =
      -2.0 / (ncolour + 2 * S) * scale;  // 2/(Nc +/- 2) for the normalization
    // of the trace in the two index rep

    for (int a = 0; a < NumGenerators; a++) {
      auto tmp = real(trace(i2indTa[a] * in)) * coefficient;
      pokeColour(h_out, tmp, a);
    }
  }
};


// Some useful type names
typedef Sp_TwoIndex<Nc, Symmetric> SpTwoIndexSymmMatrices;
typedef Sp_TwoIndex<Nc, AntiSymmetric> SpTwoIndexAntiSymmMatrices;

typedef Sp_TwoIndex<2, Symmetric> Sp2TwoIndexSymm;
typedef Sp_TwoIndex<4, Symmetric> Sp4TwoIndexSymm;

typedef Sp_TwoIndex<2, AntiSymmetric> Sp2TwoIndexAntiSymm;
typedef Sp_TwoIndex<4, AntiSymmetric> Sp4TwoIndexAntiSymm;


NAMESPACE_END(Grid);

#endif