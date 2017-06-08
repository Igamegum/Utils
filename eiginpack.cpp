#include <iostream>
#include <Eigen/Dense>
#include <assert.h>

typedef struct 
{
	Eigen::MatrixXd trans;
	std::pair<double,double> uvShift;
	std::pair<double,double> xyShift;
}DataResult;

struct EigenPack
{
	DataResult Acquaintance_transformation(const Eigen::MatrixXd,const Eigen::MatrixXd,const int );

	Eigen::MatrixXd Base_origin(const Eigen::MatrixXd,std::pair<double,double> &);

	Eigen::MatrixXd Next_matrix(const Eigen::MatrixXd);

	int Rank(const Eigen::MatrixXd);

	std::pair<Eigen::MatrixXd,Eigen::MatrixXd> Non_reflecting_transformation(const Eigen::MatrixXd,const Eigen::MatrixXd,const int);

	Eigen::MatrixXd Reflecting_transformation(const Eigen::MatrixXd,const Eigen::MatrixXd);

	Eigen::VectorXd Cons_vector(const Eigen::MatrixXd);

	Eigen::MatrixXd Pseudo_inverse(const Eigen::MatrixXd);

	std::pair<Eigen::MatrixXd,Eigen::MatrixXd> MakeForm(Eigen::MatrixXd);

	double Condiction(const Eigen::MatrixXd);

	double Singular_value(const Eigen::MatrixXd);

};


DataResult EigenPack::Acquaintance_transformation(const Eigen::MatrixXd A,
											const Eigen::MatrixXd B,
											const int K) {
//	if(A.cols() != 2 || A.rows() >= K || B.cols() !=2 || B.rows() >= K) return ;

	assert(A.cols() == 2 && A.rows() < K && B.cols() ==2 && B.rows() == K);

	std::pair<double,double> compensate_A;
	std::pair<double,double> compensate_B;

	Eigen::MatrixXd A_shift = Base_origin(A,compensate_A);
	Eigen::MatrixXd B_shift = Base_origin(B,compensate_B);

	
	typedef std::pair<Eigen::MatrixXd,Eigen::MatrixXd> EMM;

	EMM	nrt_one = Non_reflecting_transformation(A_shift,B_shift,2);

	Eigen::MatrixXd A_y = A;
	Eigen::MatrixXd B_y = B;
	for(int i = 0; i < A_y.rows(); ++i){
		A_y(i,0) *= -1.0 ;
	}
	for(int i = 0; i < B_y.rows(); ++i){
		B_y(i,0) *= -1.0 ;
	}

	EMM nrt_two = Non_reflecting_transformation(A_y,B_y,2);

	Eigen::MatrixXd TreflectY = Eigen::MatrixXd::Identity(3,3);
	TreflectY(0,0) = -1.0;

	EMM trans_two = MakeForm(nrt_two.first * TreflectY);

	Eigen::MatrixXd re_one = Reflecting_transformation(A_shift,nrt_one.first);

	double  normal_1 = Singular_value(re_one - B_shift);

	Eigen::MatrixXd re_two = Reflecting_transformation(A_shift,nrt_two.first);

	double  normal_2 = Singular_value(re_two - B_shift);

	DataResult result;
	if(normal_1 <= normal_2){
		result.trans = nrt_one.first;
	}else{
		result.trans = trans_two.first;
	}

	result.uvShift = compensate_A;
	result.xyShift = compensate_B;

	return result;
}

double EigenPack::Singular_value(const Eigen::MatrixXd X)
{
	if(X.cols() == 1 || X.rows() == 1){
		
		double sum = 0.0;

		for(int i = 0; i < X.rows(); ++i){
			for(int j = 0; j < X.cols(); ++j){
				sum += (X(i,j) * X(i,j));
			}
		}
		return sqrt(sum);
	}else{
		typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> Matxd;

		Eigen::JacobiSVD<Matxd> svd(X,Eigen::ComputeFullU | Eigen::ComputeFullV);

		double eps = 1e-8;

		Matxd singularValues_inv = svd.singularValues();

		double max_value = -1e8;
		for(long i = 0; i < X.cols(); ++i)
		{
			max_value = std::max(max_value,singularValues_inv(i));
		}
		return max_value;
	}

	return 0.0;
}

Eigen::MatrixXd EigenPack::Reflecting_transformation(const Eigen::MatrixXd X,const Eigen::MatrixXd M)
{
	Eigen::MatrixXd E(X.rows(),X.cols() + 1);

	for(int i = 0; i < X.rows(); ++i)
	{
		for(int j = 0; j < X.cols(); ++j)
		{
			E(i,j) = X(i,j);
		}
		E(i,X.cols() + 1) = 1;
	}

	Eigen::MatrixXd U_res = E * M;
	Eigen::MatrixXd U (U_res.rows(),U_res.cols());

	for(int i = 0; i < U.rows(); ++i)
	{
		for(int j = 0; j < U.cols(); ++j)
		{
			U(i,j) = U_res(i,j);
		}
	}

	return U;
}
Eigen::MatrixXd EigenPack::Base_origin(const Eigen::MatrixXd X,std::pair<double,double> & bias)
{

	assert(X.cols() == 2);
	Eigen::MatrixXd ans = X;

	double min_row = X(0,0);
	double max_row = X(0,0);

	double min_col = X(0,1);
	double max_col = X(0,1);


	for(int i = 1 ; i < X.rows() ; ++i) 
	{
		min_row = std::min(min_row,X(i,0));
		max_row = std::max(max_row,X(i,0));

		min_row = std::min(min_row,X(i,1));
		max_row = std::max(max_row,X(i,1));
	}

	double center_row = (min_row + max_row) / 2;
	double center_col = (min_col + max_col) / 2;

	std::pair<double,double> center = std::make_pair(center_row,center_col);
	std::pair<double,double> span = std::make_pair(max_row - min_row,max_col - min_col);

	
	std::pair<double,double> compensate(0.0,0.0);

	if( (span.first > 0 && std::abs(center.first / span.first) > 1e3 ) || 
			(span.second > 0 && std::abs(center.second / span.second) > 1e3 ) )
	{
		compensate = center;

		for(int i = 0 ; i < ans.rows(); ++i)
		{
			ans(i,0) -= compensate.first;
			ans(i,1) -= compensate.second;
		}
	}

	bias = compensate;

	return ans;
}
Eigen::VectorXd EigenPack::Cons_vector(const Eigen::MatrixXd X)
{
	assert(X.cols() == 2);
	Eigen::VectorXd ans(X.rows() * 2 );

	for(int i = 0; i < X.rows() ; ++i){
		ans(i) = X(i,0);
	}
	for(int i = X.rows(); i < X.rows() * 2 ; ++i){
		ans(i) = X(i,1);
	}

	return ans;
}
Eigen::MatrixXd EigenPack::Next_matrix(const Eigen::MatrixXd X)
{
	assert(X.cols() == 2 );
	Eigen::MatrixXd ans(X.rows() + X.rows(),4);
	
	for(int i = 0 ; i < X.rows(); ++i){
		ans(i,0) = X(i,0);
		ans(i,1) = X(i,1);
		ans(i,2) = 1;
		ans(i,3) = 0;
	}

	for(int i = X.rows(); i < X.rows() * 2 ; ++i)
	{
		ans(i,0) = X(i,1);
		ans(i,1) = -1.0 * X(i,0);
		ans(i,2) = 0;
		ans(i,3) = 1;
	}

	return ans;
}
int EigenPack::Rank(const Eigen::MatrixXd X)
{
	Eigen::EigenSolver<Eigen::MatrixXd> es(X);

	Eigen::EigenSolver<Eigen::MatrixXd>::EigenvalueType  values = es.eigenvalues();

	int Count = 0;
	for(int i = 0 ; i < values.rows(); ++i)
	{
		for(int j = 0; j < values.cols(); ++j)
		{
			if(values(i,j) != 0.0){
				++Count; break;
			}
		}
	}

	return Count;

}

Eigen::MatrixXd EigenPack::Pseudo_inverse(const Eigen::MatrixXd X)
{
	typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> Matxd;

	Eigen::JacobiSVD<Matxd> svd(X,Eigen::ComputeFullU | Eigen::ComputeFullV);

	double eps = 1e-8;

	Matxd singularValues_inv = svd.singularValues();
	
	for(long i = 0; i < X.cols(); ++i)
	{
		if(singularValues_inv(i) > eps)
		{
			singularValues_inv(i) = 1.0 / singularValues_inv(i) ;
		}else{
			singularValues_inv(i) = 0;
		}
	}

	Matxd result = svd.matrixV()*singularValues_inv.asDiagonal()*svd.matrixU().transpose();

	Eigen::MatrixXd ans(result.rows(),result.cols());

	for(int i = 0 ; i < result.rows(); ++i)
	{
		for(int j = 0; j <result.cols(); ++j)
		{
			ans(i,j) = result(i,j);
		}
	}

	return ans;

}

std::pair<Eigen::MatrixXd , Eigen::MatrixXd> EigenPack::MakeForm(const Eigen::MatrixXd A)
{
	double min_eps = 1e-18;
	double max_eps = 1e18;

	for(int i = 0; i < A.rows(); ++i)
	{
		for(int j = 0; j < A.cols() ;++j)
		{
			if( std::abs(A(i,j)) < min_eps || std::abs(A(i,j)) > max_eps ) {
				return std::make_pair(Eigen::MatrixXd::Zero(A.rows(),A.cols()),Eigen::MatrixXd::Zero(A.rows(),A.cols()));
			}
		}
	}

	if( A.rows() - 1 < 1 || A.cols() != A.rows() ) {
		return std::make_pair(Eigen::MatrixXd::Zero(A.rows(),A.cols()),Eigen::MatrixXd::Zero(A.rows(),A.cols()));
	}

	bool flag = true;
	for(int i = 0 ; i < A.rows() -1 ; ++i){
		if( A(i,A.cols() - 1) != 0 ){
			flag = false; break;
		}
	}

	if(flag && A(A.rows() - 1, A.cols() - 1) == 1 ){
		return std::make_pair(Eigen::MatrixXd::Zero(A.rows(),A.cols()),Eigen::MatrixXd::Zero(A.rows(),A.cols()));
	}

	if(Condiction(A) > 1e9){
		return std::make_pair(Eigen::MatrixXd::Zero(A.rows(),A.cols()),Eigen::MatrixXd::Zero(A.rows(),A.cols()));
	}

	return std::make_pair(A,Pseudo_inverse(A));
}

double EigenPack::Condiction(const Eigen::MatrixXd X )
{
	typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> Matxd;

	Eigen::JacobiSVD<Matxd> svd(X,Eigen::ComputeFullU | Eigen::ComputeFullV);

	Matxd singularValues_inv = svd.singularValues();

	bool flag = false;

	double max_num = -1.0;
	double min_num = 1e10;

	for(int i = 0; i < std::min(singularValues_inv.rows(),singularValues_inv.cols()); ++i)
	{
		if(singularValues_inv(i,i) <= 0.0){
			flag = true;
			break;
		}else{
			max_num = std::max(max_num,singularValues_inv(i,i));
			min_num = std::min(min_num,singularValues_inv(i,i));
		}
	}

	return flag?1e10:(max_num / min_num);
}

std::pair<Eigen::MatrixXd,Eigen::MatrixXd> EigenPack::Non_reflecting_transformation(const Eigen::MatrixXd A,
											  const Eigen::MatrixXd B,
											  const int K)
{
	Eigen::MatrixXd X = Next_matrix(A);
	Eigen::VectorXd U = Cons_vector(A);

	if(Rank(X) < 2 * K) {
		return std::make_pair(Eigen::MatrixXd::Zero(A.rows(),A.cols()),Eigen::MatrixXd::Zero(A.rows(),A.cols()));
	}

	Eigen::MatrixXd R = U * Pseudo_inverse(X);

	assert(R.cols() == 1);

	Eigen::MatrixXd T(3,3);

	T << R(0,0) , -1.0 * R(1,0),0,
	     R(1,0) , +1.0 * R(0,0),0,
	     R(2,0) , +1.0 * R(3,0),1;

	Eigen::MatrixXd Tinv = Pseudo_inverse(T);

	assert(Tinv.rows() >= 3 && Tinv.cols() >= 3);
	assert(Tinv(0,2) == 0 && Tinv(1,2) == 0 && Tinv(2,2) == 1);

	return MakeForm(Tinv);

}
int main()
{
	return 0;
}
