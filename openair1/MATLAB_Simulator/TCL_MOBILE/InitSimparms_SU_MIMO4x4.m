function [ simparms ] = InitSimparms_SU_MIMO4x4( n_tx, n_rx, n_l, mcs, n_frames, SNRdB )
%INITSIMPARMS initialize simparms struct for SU-MIMO scheme with a single
%spatially multiplexed codeword mapped onto four layers. 

% INPUT:
%   nb_antennas_tx = Number of transmit antennas, max = 4
%   nb_antennas_rx = Number of receive antennas per UE, max = 4
%   MCS(2) = Modulation and Coding Scheme [1,...,27]
%   n_frames = Number of frames to simulate
%   SNRdB = SNR in dB


j = sqrt(-1);

%% Custom parameters 
simparms.nb_antennas_tx = n_tx; % actual number of TX antennas
simparms.nb_antennas_rx = n_rx; % actual number of RX antennas
simparms.n_frames = n_frames;
simparms.snr_db = SNRdB;
simparms.num_layers = n_l;


%% Constant parameters
simparms.NB_ANTENNAS_TX = 4; % max number of TX antennas
simparms.NB_ANTENNAS_RX = 4; % max number of RX antennas
simparms.snr = 10.^(SNRdB/10);
simparms.num_pdcch_symbols = 3;
simparms.subframe = 7;
simparms.abstraction_flag = 0;
simparms.nb_rb = 25; % fixed number of RB per OFDM symbol (25x2)
simparms.rb_alloc =hex2dec('1FFF'); %hex2dec('1FFFF'); % For 50 RBs
simparms.MAX_TURBO_ITERATIONS = 5;
simparms.nb_re =3000*simparms.num_layers; % there are 3000 REs in subframe 7
simparms.frame_type = 1; % TDD frame
simparms.mode1_flag = 0; % no is TM1 active
simparms.Ncp = 0; % Normal CP
simparms.ofdm_symbol_size = 512;
simparms.nb_prefix_samples = 144;
simparms.log2_maxh = 16;
simparms.dl_power_offset= 1; % no 3dB power offset
simparms.nb_slots = 14;
simparms.nb_re_per_symbol = simparms.nb_rb*12;
simparms.nb_re_per_frame = simparms.nb_slots*simparms.nb_re_per_symbol;
simparms.tseeds = SetTausSeed(1); % taus seeds
simparms.Nsoft = 1827072;

% Turbo decoder: set the pointer to the td tables allocated by init_td()
DlschDecodingInit();
% Init codewords
simparms.cw = InitCodeword(simparms,mcs(1));
simparms.first_symbol_flag= 1;


end

function cw = InitCodeword(simparms,mcs)

% Constellation
j = sqrt(-1);
amp = 1/32;
rho_a_b_q15=512 % this is in Q15
rho_a_b=0.0156 % this is in normal scale
ONE_OVER_SQRT_2_Q15 = (1/(sqrt(2)))*(2^15)
QPSK_gain_q15=floor(rho_a_b_q15*ONE_OVER_SQRT_2_Q15/(2^15));
QPSK_gain=rho_a_b*(1/sqrt(2));
Q4 = QPSK_gain*[ 1+j,  1- j,  -1+j, -1-j];
Q16 = rho_a_b*[ 1+j,  1+3*j,  3+j,  3+3*j,  1-j,  1-3*j,  3-j,  3-3*j,...
           -1+j, -1+3*j, -3+j, -3+3*j, -1-j, -1-3*j, -3-j, -3-3*j]./sqrt(10);
Q64 = rho_a_b*[ 3+3*j, 3+j, 1+3*j, 1+  j, 3+5*j, 3+7*j, 1+5*j, 1+7*j,...
	        5+3*j, 5+j, 7+  j, 7+3*j, 5+5*j, 5+7*j, 7+5*j, 7+7*j,...
	        3-3*j, 3-j, 1-3*j, 1-  j, 3-5*j, 3-7*j, 1-5*j, 1-7*j,...
	        5-3*j, 5-j, 7-3*j, 7-  j, 5-5*j, 5-7*j, 7-5*j, 7-7*j,...
	       -3+3*j,-3+j,-1+3*j,-1+  j,-3+5*j,-3+7*j,-1+5*j,-1+7*j,...
	       -5+3*j,-5+j,-7+3*j,-7+  j,-5+5*j,-5+7*j,-7+5*j,-7+7*j,...
	       -3-3*j,-3-j,-1-3*j,-1-  j,-3-5*j,-3-7*j,-1-5*j,-1-7*j,...
	       -5-3*j,-5-j,-7-3*j,-7-  j,-5-5*j,-5-7*j,-7-5*j,-7-7*j]./sqrt(42);

if (mcs <= 9)
		cw.mod_order = 2;
		cw.base2 = [2 1];
		cw.const = Q4;
        cw.scale_coef=QPSK_gain;
		
elseif (mcs <= 16)			
		cw.mod_order = 4;
		cw.base2 = [8 4 2 1];
		cw.const = Q16;
        cw.scale_coef=rho_a_b*sqrt(4/sqrt(10));
		
elseif (mcs <= 28)
		cw.mod_order = 6;
		cw.base2 = [32 16 8 4 2 1];
		cw.const = Q64;
        cw.scale_coef=rho_a_b*sqrt(6/sqrt(42));
		
else
		error('invalid MCS: %d\n', mcs);
		
end

cw.mcs = mcs;

cw.Kmimo = 2;
cw.harq_pid = 0; % First HARQ round
cw.Mdlharq = 1;  % Number of HARQ rounds
cw.rvidx = 0;
cw.round = 0;
cw.Ndi = 1;
cw.G = simparms.nb_re*cw.mod_order;%*codeword.Nl; % Number of softbits
cw.TBS = GetTbs(simparms, cw)

end
