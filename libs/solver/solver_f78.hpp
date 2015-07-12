#pragma once

#include "solver.hpp"

NS_ODE_BEGIN

template<class System>
class controlled_runge_kutta<Steppers::RKF78,System>
{
protected:
	typedef typename System::value_type value_type;
	typedef typename System::state_type state_type;
	typedef typename System::deriv_type deriv_type;
	typedef typename System::time_type time_type;

public:

	controlled_runge_kutta(const value_type eps_abs,const value_type eps_rel)
	: m_error_checker( eps_abs, eps_rel) ,
	  m_first_call( true )
	{ }

	controlled_step_result try_step( System &system , state_type &x , time_type &t , time_type &dt )
	{
		system.rhs( x , m_dxdt , t );
		controlled_step_result res = try_step2( system , x , m_dxdt , t , m_xnew , dt );
		dt=std::min(dt,system.max_dt); // added for maximum step size
		if( res == success )
		{
			x=m_xnew;
			// m_dxdt=m_dxdtnew;
		}
		return res;
	}

	//call 4
	controlled_step_result try_step2( System &system , const state_type &x_in , const deriv_type &dxdt_in , time_type &t ,
			state_type &x_out , time_type &dt)
	{
		do_step( system , x_in , dxdt_in , t , x_out , dt , m_xerr );

		// this potentially overwrites m_x_err! (standard_error_checker does, at least)
		value_type max_rel_err = m_error_checker.error(  x_in , dxdt_in , m_xerr , dt );

		if( max_rel_err > 1.0 )
		{
			// error too large - decrease dt ,limit scaling factor to 0.2 and reset state
			// simplified: max( 9/10*pow( max_rel_err , -1/( m_stepper.error_order_value - 1) ) ,1/5 );
			dt *= std::max( value_type( value_type(9)/value_type(10) *
						pow( max_rel_err , value_type(-1) / ( error_order_value - 1 ) ) ) ,
						value_type( value_type(1)/value_type(5)) );
			return fail;
		}
		else
		{
			t += dt;
			if( max_rel_err < 0.5 )
			{   //error too small - increase dt and keep the evolution and limit scaling factor to 5.0
				// error should be > 0
				max_rel_err = std::max( value_type( pow( value_type(5.0) , -value_type(stepper_order_value) ) ) , max_rel_err );
				// simplified: 9/10 * pow( max_rel_err , -1 / m_stepper.stepper_order_value )
				dt *= value_type( value_type(9)/value_type(10) * pow( max_rel_err , value_type(-1) / stepper_order_value ) );
			}
			return success;
		}
	}

private:

	default_error_checker<System> m_error_checker;

	deriv_type m_dxdt;
	state_type m_xerr;
	state_type m_xnew;
	deriv_type m_dxdtnew;
	bool m_first_call;

public:
	const unsigned short stage_count_value = 13; // where is it used????
	const unsigned short order_value = 8; // where is it used????
	const unsigned short stepper_order_value = 8;
	const unsigned short error_order_value = 7;

	void do_step(System &system , const state_type &x_in , const deriv_type &dxdt_in , time_type t ,
			state_type &x_out , time_type dt , state_type &xerr )
	{
		const value_type a2 = value_type( 2 ) / value_type( 27 );
		const value_type a3 = value_type( 1 ) / value_type( 9 );
		const value_type a4 = value_type( 1 ) / value_type( 6 );
		const value_type a5 = value_type( 5 ) / value_type( 12 );
		const value_type a6 = value_type( 1 ) / value_type( 2 );
		const value_type a7 = value_type( 5 ) / value_type( 6 );
		const value_type a8 = value_type( 1 ) / value_type( 6 );
		const value_type a9 = value_type( 2 ) / value_type( 3 );
		const value_type a10 = value_type( 1 ) / value_type( 3 );
		const value_type a11 = value_type( 1 );
		const value_type a13 = value_type( 1 );

		const value_type b21 = value_type( 2 ) / value_type( 27 );

		const value_type b31 = value_type( 1 ) / value_type( 36 );
		const value_type b32 = value_type( 1 ) / value_type( 12 );

		const value_type b41 = value_type( 1 ) / value_type( 24 );
		const value_type b43 = value_type( 1 ) / value_type( 8 );

		const value_type b51 = value_type( 5 ) / value_type( 12 );
		const value_type b52 = value_type( 0 );
		const value_type b53 = value_type( -25 ) / value_type( 16 );
		const value_type b54 = value_type( 25 ) / value_type( 16 );

		const value_type b61 = value_type( 1 ) / value_type( 20 );
		const value_type b64 = value_type( 1 ) / value_type( 4 );
		const value_type b65 = value_type( 1 ) / value_type( 5 );

		const value_type b71 = value_type( -25 )/value_type( 108 );
		const value_type b74 = value_type( 125 ) / value_type( 108 );
		const value_type b75 = value_type( -65 ) / value_type( 27 );
		const value_type b76 = value_type( 125 ) / value_type( 54 );

		const value_type b81 = value_type( 31 ) / value_type( 300 );
		const value_type b85 = value_type( 61 ) / value_type( 225 );
		const value_type b86 = value_type( -2 ) / value_type( 9 );
		const value_type b87 = value_type( 13 ) / value_type( 900 );

		const value_type b91 = value_type( 2 );
		const value_type b94 = value_type( -53 ) / value_type( 6 );
		const value_type b95 = value_type(704 ) / value_type( 45 );
		const value_type b96 = value_type( -107 ) / value_type( 9 );
		const value_type b97 = value_type( 67 ) / value_type( 90 );
		const value_type b98 = value_type( 3 );

		const value_type b10_1 = value_type( -91 ) / value_type( 108 );
		const value_type b10_4 = value_type( 23 ) / value_type( 108 );
		const value_type b10_5 = value_type( -976 ) / value_type( 135 );
		const value_type b10_6 = value_type( 311 ) / value_type( 54 );
		const value_type b10_7 = value_type( -19 ) / value_type( 60 );
		const value_type b10_8 = value_type( 17 ) / value_type( 6 );
		const value_type b10_9 = value_type( -1 ) / value_type( 12 );

		const value_type b11_1 = value_type( 2383 ) / value_type( 4100 );
		const value_type b11_4 = value_type( -341 ) / value_type( 164 );
		const value_type b11_5 = value_type( 4496 ) / value_type( 1025 );
		const value_type b11_6 = value_type( -301 ) / value_type( 82 );
		const value_type b11_7 = value_type( 2133 ) / value_type( 4100 );
		const value_type b11_8 = value_type( 45 ) / value_type( 82 );
		const value_type b11_9 = value_type( 45 ) / value_type( 164 );
		const value_type b11_10 = value_type( 18 ) / value_type( 41 );

		const value_type b12_1 = value_type( 3 ) / value_type( 205 );
		const value_type b12_6 = value_type( -6 ) / value_type( 41 );
		const value_type b12_7 = value_type( -3 ) / value_type( 205 );
		const value_type b12_8 = value_type( -3 ) / value_type( 41 );
		const value_type b12_9 = value_type( 3 ) / value_type( 41 );
		const value_type b12_10 = value_type( 6 ) / value_type( 41 );

		const value_type b13_1 = value_type( -1777 ) / value_type( 4100 );
		const value_type b13_4 = value_type( -341 ) / value_type( 164 );
		const value_type b13_5 = value_type( 4496 ) / value_type( 1025 );
		const value_type b13_6 = value_type( -289 ) / value_type( 82 );
		const value_type b13_7 = value_type( 2193 ) / value_type( 4100 );
		const value_type b13_8 = value_type( 51 ) / value_type( 82 );
		const value_type b13_9 = value_type( 33 ) / value_type( 164 );
		const value_type b13_10 = value_type( 12 ) / value_type( 41 );
		const value_type b13_11 = value_type( 0 );
		const value_type b13_12 = value_type( 1 );

		const value_type c6 = value_type( 34 ) / value_type( 105 );
		const value_type c7 = value_type( 9 ) / value_type( 35 );
		const value_type c8 = value_type( 9 ) / value_type( 35 );
		const value_type c9 = value_type( 9 ) / value_type( 280 );
		const value_type c10 = value_type( 9 ) / value_type( 280 );
		const value_type c12 = value_type( 41 ) / value_type( 840 );
		const value_type c13 = value_type( 41 ) / value_type( 840 );

		const value_type dc1 = value_type( 0 ) - value_type( 41 ) / value_type( 840 );
		const value_type dc11 = value_type( 0 ) - value_type( 41 ) / value_type( 840 );
		const value_type dc12 = value_type( 41 ) / value_type( 840 );
		const value_type dc13 = value_type( 41 ) / value_type( 840 );

		m_x_tmp= x_in + dt*b21 * dxdt_in;

		system.rhs( m_x_tmp , m_k2 , t + dt*a2 );
		m_x_tmp= x_in + dt*b31*dxdt_in + dt*b32*m_k2; // why cannot factor dt????

		system.rhs( m_x_tmp , m_k3 , t + dt*a3 );
		m_x_tmp=x_in+dt*b41*dxdt_in+dt*b43*m_k3;

		system.rhs( m_x_tmp, m_k4 , t + dt*a4 );
		m_x_tmp=x_in+dt*b51*dxdt_in+dt*b52*m_k2+ dt*b53*m_k3+dt*b54*m_k4;

		system.rhs( m_x_tmp , m_k5 , t + dt*a5 );
		m_x_tmp=x_in + dt*b61*dxdt_in + dt*b64*m_k4 + dt*b65*m_k5 ;

		system.rhs( m_x_tmp , m_k6 , t + dt*a6 );
		m_x_tmp=x_in + dt*b71*dxdt_in + dt*b74*m_k4 + dt*b75*m_k5 + dt*b76*m_k6;

		system.rhs( m_x_tmp , m_k7 , t + dt*a7 );
		m_x_tmp=x_in + dt*b81*dxdt_in + dt*b85*m_k5 + dt*b86*m_k6 + dt*b87*m_k7;

		system.rhs( m_x_tmp , m_k8 , t + dt*a8 );
		m_x_tmp=x_in + dt*b91*dxdt_in + dt*b94*m_k4 + dt*b95*m_k5 + dt*b96*m_k6 + dt*b97*m_k7 + dt*b98*m_k8;

		system.rhs( m_x_tmp , m_k9 , t + dt*a9 );
		m_x_tmp=x_in + dt*b10_1*dxdt_in + dt*b10_4*m_k4 + dt*b10_5*m_k5 + dt*b10_6*m_k6 + dt*b10_7*m_k7 + dt*b10_8*m_k8 + dt*b10_9*m_k9;
		
		system.rhs( m_x_tmp , m_k10 , t + dt*a10 );
		m_x_tmp=x_in + dt*b11_1*dxdt_in + dt*b11_4*m_k4 + dt*b11_5*m_k5 + dt*b11_6*m_k6 + dt*b11_7*m_k7 + dt*b11_8*m_k8 + dt*b11_9*m_k9 + dt*b11_10*m_k10;

		system.rhs( m_x_tmp , m_k11 , t + dt*a11 );
		m_x_tmp=x_in + dt*b12_1*dxdt_in + dt*b12_6*m_k6 + dt*b12_7*m_k7 + dt*b12_8*m_k8 + dt*b12_9*m_k9 + dt*b12_10*m_k10;

		system.rhs( m_x_tmp , m_k12 , t );
		m_x_tmp=x_in + dt*b13_1*dxdt_in + dt*b13_4*m_k4 + dt*b13_5*m_k5 + dt*b13_6*m_k6 + dt*b13_7*m_k7 + dt*b13_8*m_k8 + dt*b13_9*m_k9 + dt*b13_10*m_k10 + dt*b13_11*m_k11 + dt*b13_12*m_k12;

		system.rhs( m_x_tmp , m_k13 , t + dt*a13 ); // a13=1
		x_out= x_in + dt*c6*m_k6 + dt*c7*m_k7 + dt*c8*m_k8 + dt*c9*m_k9 + dt*c10*m_k10 + dt*c12*m_k12 + dt*c13*m_k13 ;

		//error estimate
		xerr=dt*dc1*dxdt_in + dt*dc11*m_k11 + dt*dc12*m_k12 + dt*dc13*m_k13;
	}

private:
	state_type m_x_tmp;
	deriv_type m_k2 , m_k3 , m_k4 , m_k5 , m_k6 , m_k7 , m_k8 , m_k9 , m_k10 , m_k11 , m_k12 , m_k13 ;
};

NS_ODE_END
