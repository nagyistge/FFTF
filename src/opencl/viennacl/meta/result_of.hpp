#ifndef VIENNACL_META_RESULT_OF_HPP_
#define VIENNACL_META_RESULT_OF_HPP_

/* =========================================================================
   Copyright (c) 2010-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.
   Portions of this software are copyright by UChicago Argonne, LLC.

                            -----------------
                  ViennaCL - The Vienna Computing Library
                            -----------------

   Project Head:    Karl Rupp                   rupp@iue.tuwien.ac.at
               
   (A list of authors and contributors can be found in the PDF manual)

   License:         MIT (X11), see file LICENSE in the base directory
============================================================================= */

/** @file viennacl/meta/result_of.hpp
    @brief A collection of compile time type deductions
*/

#include <string>
#include <fstream>
#include <sstream>
#include "viennacl/forwards.h"


#ifdef VIENNACL_WITH_UBLAS  
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#endif

#ifdef VIENNACL_WITH_EIGEN  
#include <Eigen/Core>
#include <Eigen/Sparse>
#endif

#ifdef VIENNACL_WITH_MTL4
#include <boost/numeric/mtl/mtl.hpp>
#endif

#include <vector>
#include <map>

namespace viennacl
{
    namespace result_of
    {
      //
      // Retrieve alignment from vector
      //
      /** @brief Retrieves the alignment from a vector. Deprecated - will be replaced by a pure runtime facility in the future. */
      template <typename T>
      struct alignment
      {
        typedef typename T::ERROR_ARGUMENT_PROVIDED_IS_NOT_A_VECTOR_OR_A_MATRIX   error_type;
        enum { value = 1 };
      };
      
      /** \cond */
      template <typename T>
      struct alignment<const T>
      {
        enum { value = alignment<T>::value };
      };
      
      template <typename SCALARTYPE, unsigned int ALIGNMENT>
      struct alignment< vector<SCALARTYPE, ALIGNMENT> >
      {
        enum { value = ALIGNMENT };
      };

      template <typename T>
      struct alignment< vector_range<T> >
      {
        enum { value = alignment<T>::value };
      };

      template <typename T>
      struct alignment< vector_slice<T> >
      {
        enum { value = alignment<T>::value };
      };

      // support for a*x with scalar a and vector x
      template <typename LHS, typename RHS, typename OP>
      struct alignment< vector_expression<LHS, RHS, OP> >
      {
        enum { value = alignment<LHS>::value };
      };


      // Matrices
      template <typename SCALARTYPE, typename F, unsigned int ALIGNMENT>
      struct alignment< matrix<SCALARTYPE, F, ALIGNMENT> >
      {
        enum { value = ALIGNMENT };
      };

      template <typename T>
      struct alignment< matrix_range<T> >
      {
        enum { value = alignment<T>::value };
      };

      template <typename T>
      struct alignment< matrix_slice<T> >
      {
        enum { value = alignment<T>::value };
      };
      
      template <typename LHS, typename RHS>
      struct alignment< matrix_expression<LHS, RHS, op_trans> >
      {
        enum { value = alignment<LHS>::value };
      };
      /** \endcond */
      
      //
      // Majority specifier for matrices (row_major, column_major)
      //
      /** @brief Returns the orientation functor tag (either row_major or column_major) of a matrix */
      template <typename T>
      struct orientation_functor
      {
        typedef typename T::ERROR_ARGUMENT_PROVIDED_IS_NOT_A_MATRIX     type;
      };

      /** \cond */
      template <typename T>
      struct orientation_functor<const T>
      {
        typedef typename orientation_functor<T>::type  type;
      };
      
      template <typename SCALARTYPE, typename F, unsigned int ALIGNMENT>
      struct orientation_functor< matrix<SCALARTYPE, F, ALIGNMENT> >
      {
        typedef F     type;
      };

      template <typename T>
      struct orientation_functor< matrix_range<T> >
      {
        typedef typename orientation_functor<T>::type  type;
      };

      template <typename T>
      struct orientation_functor< matrix_slice<T> >
      {
        typedef typename orientation_functor<T>::type  type;
      };

      template <typename LHS, typename RHS>
      struct orientation_functor< matrix_expression<LHS, RHS, op_trans> >
      {
        typedef typename orientation_functor<LHS>::type  type;
      };
      /** \endcond */
      
      
      //
      // Retrieve size_type 
      //
      /** @brief Generic meta-function for retrieving the size_type associated with type T */
      template <typename T>
      struct size_type
      {
        typedef typename T::size_type   type;
      };

      /** \cond */
      #ifdef VIENNACL_WITH_EIGEN
      template <class T, int a, int b, int c, int d, int e>
      struct size_type< Eigen::Matrix<T, a, b, c, d, e> >
      {
        typedef std::size_t   type;
      };
      
      template <>
      struct size_type<Eigen::VectorXf>
      {
        typedef std::size_t   type;
      };
      
      template <>
      struct size_type<Eigen::VectorXd>
      {
        typedef std::size_t   type;
      };

      template <typename T, int options>
      struct size_type<Eigen::SparseMatrix<T, options> >
      {
        typedef std::size_t   type;
      };
      #endif
      /** \endcond */
      
      //
      // Retrieve value_type:
      //
      /** @brief Generic helper function for retrieving the value_type associated with type T */
      template <typename T>
      struct value_type
      {
        typedef typename T::value_type    type; 
      };
      
      /** \cond */
#ifdef VIENNACL_WITH_EIGEN  
      template <>
      struct value_type<Eigen::MatrixXf>
      {
        typedef Eigen::MatrixXf::RealScalar    type; 
      };
      
      template <>
      struct value_type<Eigen::MatrixXd>
      {
        typedef Eigen::MatrixXd::RealScalar    type; 
      };

      template <typename ScalarType, int option>
      struct value_type<Eigen::SparseMatrix<ScalarType, option> >
      {
        typedef ScalarType    type; 
      };

      template <>
      struct value_type<Eigen::VectorXf>
      {
        typedef Eigen::VectorXf::RealScalar    type; 
      };

      template <>
      struct value_type<Eigen::VectorXd>
      {
        typedef Eigen::VectorXd::RealScalar    type; 
      };
      
#endif
      /** \endcond */
      
      
      //
      // Retrieve cpu value_type:
      //
      /** @brief Helper meta function for retrieving the main RAM-based value type. Particularly important to obtain T from viennacl::scalar<T> in a generic way. */
      template <typename T>
      struct cpu_value_type
      {
        typedef typename T::ERROR_CANNOT_DEDUCE_CPU_SCALAR_TYPE_FOR_T    type; 
      };

      /** \cond */
      template <typename T>
      struct cpu_value_type<const T>
      {
        typedef typename cpu_value_type<T>::type    type;
      };
      
      template <>
      struct cpu_value_type<float>
      {
        typedef float    type; 
      };
      
      template <>
      struct cpu_value_type<double>
      {
        typedef double    type; 
      };
      
      template <typename T>
      struct cpu_value_type<viennacl::scalar<T> >
      {
        typedef T    type; 
      };

      template <typename T>
      struct cpu_value_type<viennacl::vector_base<T> >
      {
        typedef T    type; 
      };
      
      template <typename T, unsigned int ALIGNMENT>
      struct cpu_value_type<viennacl::vector<T, ALIGNMENT> >
      {
        typedef T    type; 
      };
      
      template <typename T>
      struct cpu_value_type<viennacl::vector_range<T> >
      {
        typedef typename cpu_value_type<T>::type    type; 
      };

      template <typename T>
      struct cpu_value_type<viennacl::vector_slice<T> >
      {
        typedef typename cpu_value_type<T>::type    type; 
      };
      
      template <typename T1, typename T2, typename OP>
      struct cpu_value_type<viennacl::vector_expression<const T1, const T2, OP> >
      {
        typedef typename cpu_value_type<T1>::type    type; 
      };
      
      template <typename T1, typename T2, typename OP>
      struct cpu_value_type<const viennacl::vector_expression<const T1, const T2, OP> >
      {
        typedef typename cpu_value_type<T1>::type    type; 
      };
      
      
      template <typename T, typename F>
      struct cpu_value_type<viennacl::matrix_base<T, F> >
      {
        typedef T    type; 
      };
      
      template <typename T, typename F, unsigned int ALIGNMENT>
      struct cpu_value_type<viennacl::matrix<T, F, ALIGNMENT> >
      {
        typedef T    type; 
      };
      
      template <typename T>
      struct cpu_value_type<viennacl::matrix_range<T> >
      {
        typedef typename cpu_value_type<T>::type    type; 
      };

      template <typename T>
      struct cpu_value_type<viennacl::matrix_slice<T> >
      {
        typedef typename cpu_value_type<T>::type    type; 
      };
      
      template <typename T1, typename T2, typename OP>
      struct cpu_value_type<viennacl::matrix_expression<T1, T2, OP> >
      {
        typedef typename cpu_value_type<T1>::type    type; 
      };
      
      
      
      template <typename T>
      struct matrix_expression_internal_storage
      {
        typedef T &     type;
      };
     
      template <>
      struct matrix_expression_internal_storage<const float>
      {
        typedef float type;
      };
      
      template <>
      struct matrix_expression_internal_storage<const double>
      {
        typedef double type;
      };
      
      
      
      
      //
      // Deduce compatible vector type for a matrix type
      //

      template <typename T>
      struct vector_for_matrix
      {
        typedef typename T::ERROR_CANNOT_DEDUCE_VECTOR_FOR_MATRIX_TYPE   type;
      };

      //ViennaCL
      template <typename T, typename F, unsigned int A>
      struct vector_for_matrix< viennacl::matrix<T, F, A> >
      {
        typedef viennacl::vector<T,A>   type;
      };

      template <typename T, unsigned int A>
      struct vector_for_matrix< viennacl::compressed_matrix<T, A> >
      {
        typedef viennacl::vector<T,A>   type;
      };

      template <typename T, unsigned int A>
      struct vector_for_matrix< viennacl::coordinate_matrix<T, A> >
      {
        typedef viennacl::vector<T,A>   type;
      };

      #ifdef VIENNACL_WITH_UBLAS
      //Boost:
      template <typename T, typename F, typename A>
      struct vector_for_matrix< boost::numeric::ublas::matrix<T, F, A> >
      {
        typedef boost::numeric::ublas::vector<T>   type;
      };

      template <typename T, typename U, std::size_t A, typename B, typename C>
      struct vector_for_matrix< boost::numeric::ublas::compressed_matrix<T, U, A, B, C> >
      {
        typedef boost::numeric::ublas::vector<T>   type;
      };

      template <typename T, typename U, std::size_t A, typename B, typename C>
      struct vector_for_matrix< boost::numeric::ublas::coordinate_matrix<T, U, A, B, C> >
      {
        typedef boost::numeric::ublas::vector<T>   type;
      };
      #endif

      
      template <typename T>
      struct reference_if_nonscalar
      {
        typedef T &    type;
      };
    
      template <>
      struct reference_if_nonscalar<float>
      {
        typedef float    type;
      };

      template <>
      struct reference_if_nonscalar<const float>
      {
        typedef const float    type;
      };
      
      template <>
      struct reference_if_nonscalar<double>
      {
        typedef double    type;
      };
      
      template <>
      struct reference_if_nonscalar<const double>
      {
        typedef const double    type;
      };
      /** \endcond */
      
    } //namespace result_of
} //namespace viennacl
    

#endif
