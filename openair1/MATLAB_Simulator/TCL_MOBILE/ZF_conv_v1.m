%% Header
%==========================================================================
% This testbench simulations transmission mode 5 on subframe 7.
% Results can be compared to "./dlsim -x5 -S7 -R1 -u1" with perfect chest
% No OFDM Modulation and Demodulation is applied!
%
% Author: Sebastian Wagner
% Date: 24-07-2012
%
%==========================================================================

clear all;
close all;
clear mex;

addpath('/home/eurecom/OAI/openairinterface5g/cmake_targets/mexfiles/build')
addpath('/home/eurecom/OAI/openairinterface5g/cmake_targets/mexfiles/src/mex/mexOAI')

% profile on;
tic
%% System parameters

nt=4;
nr=4;
N = 1000; % number of frames (codewords)
nSNR =1;
SNRdB = [45];
n_layers=4;

MCS = 27;
j = sqrt(-1);
amp=1/32;

XFORMS = 0;
% LLRs are computed for 8RE make sure enough valid memory is allocated
LLR_GUARD_SAMPLES = 8*6; % max mod_order 6 for 8 REs
reimag=2;

%% Initialize simparms
simparms = InitSimparms_SU_MIMO4x4( nt, nr, n_layers, MCS, N, SNRdB);

%% Random data, same seed as in dlsim.c
[tmp simparms.tseeds] = Taus(1,simparms.tseeds);

%% Index of REs carrying data
data_idx = [901:1400 1501:2300 2401:3500 3601:4200];
data_idx_int = [1801:2800 3001:4600 4801:7000 7201:8400]; % [Re Im] format
data_idx_int_r = data_idx_int(1:2:length(data_idx_int));
data_idx_int_i = data_idx_int(2:2:length(data_idx_int));

%% Allocate memory

H = zeros(simparms.NB_ANTENNAS_RX,simparms.NB_ANTENNAS_TX);
noise = zeros(simparms.NB_ANTENNAS_RX,simparms.nb_re/n_layers);
frame_errors=zeros(length(SNRdB), 1);
throughput=zeros(length(SNRdB), 1, 'double');

llr_L0 = zeros((simparms.cw.G)/4, 1,'int16');
llr_L1 = zeros((simparms.cw.G)/4, 1,'int16');
llr_L2 = zeros((simparms.cw.G)/4, 1,'int16');
llr_L3 = zeros((simparms.cw.G)/4, 1,'int16');
LLR_demap = zeros(simparms.cw.G, 1,'int16');

% Received signal in fixed point
y_fxp = zeros(simparms.NB_ANTENNAS_RX,simparms.nb_re_per_frame,'int16');

% FXP received signal with transformed dimensions.
% This is an analog of rx_dataF[aatx*simparms.NB_ANTENNAS_RX+aarx]
y_fxp_t = zeros(reimag*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX,'int16');
y_ZF_fxp_t = zeros(reimag*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX,'int16');
y_ZF_fxp_t_norm = zeros(reimag*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX,'int16');

% Received signal after compensation
ymf0 = zeros(reimag*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
H_eff_mag0 = zeros(reimag*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
I_0 = zeros(1, 1800);
Q_0 = zeros(1, 1800);
I_1 = zeros(1, 1800);
Q_1 = zeros(1, 1800);
I_2 = zeros(1, 1800);
Q_2 = zeros(1, 1800);
I_3 = zeros(1, 1800);
Q_3 = zeros(1, 1800);

Ib_0 = zeros(1, 1800);
Qb_0 = zeros(1, 1800);
Ib_1 = zeros(1, 1800);
Qb_1 = zeros(1, 1800);
Ib_2 = zeros(1, 1800);
Qb_2 = zeros(1, 1800);
Ib_3 = zeros(1, 1800);
Qb_3 = zeros(1, 1800);

Isum_0 = zeros(1, 1800);
Qsum_0 = zeros(1, 1800);
Isum_1 = zeros(1, 1800);
Qsum_1 = zeros(1, 1800);
Isum_2 = zeros(1, 1800);
Qsum_2 = zeros(1, 1800);
Isum_3 = zeros(1, 1800);
Qsum_3 = zeros(1, 1800);

% Effective channel will contain the channel estimate at pilot positions
Heff0 = zeros(reimag*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
Hmag0 = zeros(reimag*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX, 'int16');
Hmagb0 = zeros(reimag*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
rho10 = zeros(reimag*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');

%% XFORMS
if (XFORMS)
	scrsz = get(0,'ScreenSize');
    figure('Position',[1 scrsz(4)/4 scrsz(3)/4 scrsz(4)/1])
	pl(1)=subplot(4,2,1);
    fig_llr0 = scatter(1:(simparms.cw.G/4),llr_L0,'.','YDataSource','llr_L0');  
    title('LLRs of Layer 0', 'FontSize',15);

    pl(2)=subplot(4,2,3);
	fig_llr1 = scatter(1:(simparms.cw.G/4),llr_L1,'.','YDataSource','llr_L1');
    title('LLRs of Layer 1', 'FontSize',15);
    
    pl(3)=subplot(4,2,5);
	fig_llr2 = scatter(1:(simparms.cw.G/4),llr_L2,'.','YDataSource','llr_L2');
    title('LLRs of Layer 2', 'FontSize',15);
    
    pl(4)=subplot(4,2,7);
	fig_llr3 = scatter(1:(simparms.cw.G/4),llr_L3,'.','YDataSource','llr_L3');
    title('LLRs of Layer 3', 'FontSize',15);

    
	pl(5)=subplot(4,2,2);
	fig_ymf0 = scatter(ymf0(data_idx_int_r,1),ymf0(data_idx_int_i,1),'.','XDataSource','ymf0(data_idx_int_r,1)','YDataSource','ymf0(data_idx_int_i,1)');
    hold on
    fig_IQ_0=scatter(I_0,Q_0, '.','g', 'XDataSource','I_0','YDataSource','Q_0');
    fig_IQb_0=scatter(Ib_0,Qb_0, '.','r', 'XDataSource','Ib_0','YDataSource','Qb_0');
    fig_IQsum_0=scatter(Isum_0,Qsum_0, '.','m', 'XDataSource','Isum_0','YDataSource','Qsum_0');
    %fig_mag0 = scatter(H_eff_mag0(data_idx_int_r,1),H_eff_mag0(data_idx_int_i,1),'.','XDataSource','H_eff_mag0(data_idx_int_r,1)','YDataSource','H_eff_mag0(data_idx_int_i,1)');
    title('Effective channel after ZF Layer 0', 'FontSize',15);
    hold off
    
    pl(6)=subplot(4,2,4);
	fig_ymf1 = scatter(ymf0(data_idx_int_r,6),ymf0(data_idx_int_i,6),'.','XDataSource','ymf0(data_idx_int_r,6)','YDataSource','ymf0(data_idx_int_i,6)');
    hold on
    fig_IQ_1=scatter(I_1,Q_1, '.','g', 'XDataSource','I_1','YDataSource','Q_1');
    fig_IQb_1=scatter(Ib_1,Qb_1, '.','r', 'XDataSource','Ib_1','YDataSource','Qb_1');
    fig_IQsum_1=scatter(Isum_1,Qsum_1, '.','m', 'XDataSource','Isum_1','YDataSource','Qsum_1');
    hold off
    title('Effective channel after ZF Layer 1', 'FontSize',15);
    
    
    pl(7)=subplot(4,2,6);
	fig_ymf2 = scatter(ymf0(data_idx_int_r,11),ymf0(data_idx_int_i,11),'.','XDataSource','ymf0(data_idx_int_r,11)','YDataSource','ymf0(data_idx_int_i,11)');
    hold on
    fig_IQ_2=scatter(I_2,Q_2, '.','g', 'XDataSource','I_2','YDataSource','Q_2');
    fig_IQb_2=scatter(Ib_2,Qb_2, '.','r', 'XDataSource','Ib_2','YDataSource','Qb_2');
    fig_IQsum_2=scatter(Isum_2,Qsum_2, '.','m', 'XDataSource','Isum_1','YDataSource','Qsum_2');
    hold off
    title('Effective channel after ZF Layer 2', 'FontSize',15);
    
    pl(8)=subplot(4,2,8);
	fig_ymf3 = scatter(ymf0(data_idx_int_r,16),ymf0(data_idx_int_i,16),'.','XDataSource','ymf0(data_idx_int_r,16)','YDataSource','ymf0(data_idx_int_i,16)');
    hold on
    fig_IQ_3=scatter(I_3,Q_3, '.','g', 'XDataSource','I_3','YDataSource','Q_3');
    fig_IQb_3=scatter(Ib_3,Qb_3, '.','r', 'XDataSource','Ib_3','YDataSource','Qb_3');
    fig_IQsum_3=scatter(Isum_3,Qsum_3, '.','m', 'XDataSource','Isum_3','YDataSource','Qsum_3');
    title('Effective channel after ZF Layer 3', 'FontSize',15);
    hold off
    
    linkaxes(pl, 'y')
    ylim([-2000 2000])
end

%% Encode and modulate transmit signal
% The same data is used throughout the simulation -> saves time
[data0 simparms.tseeds] = Taus(simparms.cw.TBS/8,simparms.tseeds);
% Add 4 bytes CRC
data0 = [data0; zeros(4,1,'uint8')];

edata0 = DlschEncoding(data0,simparms,simparms.cw);

%% Modulation edata_enc contains symbols, and not bits.
edata_enc(:,1)= simparms.cw.base2*double(reshape(edata0,simparms.cw.mod_order,simparms.nb_re));

% x is scaled symbol
x(:,1) = simparms.cw.const(edata_enc(:,1)+1);
%%Ptx = sum(diag(x'*x))/(simparms.nb_re); % average transmit power per RE (total over all layers)

%% Layer Mapping
  x_L1 = x(1:4:length(x));
  x_L2 = x(2:4:length(x));
  x_L3 = x(3:4:length(x));
  x_L4 = x(4:4:length(x));

  x_total_layer = [x_L1 x_L2 x_L3 x_L4].';
  
 Ptx = sum(diag(x_total_layer'*x_total_layer))/(simparms.nb_re); % average transmit power per RE (total over all layers)


%% Loop over SNRs
 for iSNR=1:length(SNRdB)
 	cSNR = simparms.snr(iSNR); % current SNR
 	%% Loop over Frames
 	for n=1:simparms.n_frames
		%% Channel
 		% Rayleigh channel, constant for whole codeblock/frame, always 4x4
 		H(1:nr,1:nt) = (randn(nr,nt) + j*randn(nr,nt))/sqrt(2);

		%% Noise
		sigma2 = (Ptx/cSNR);
		noise(1:nr,:) = sqrt(sigma2).*(randn(nr,simparms.nb_re/n_layers) + j*randn(nr,simparms.nb_re/n_layers))./sqrt(2);

		%% Received signal
		y = H*x_total_layer + noise;

        %% ZF filter
        Hh_H= H'*H;
        Hh_H_inv=inv(Hh_H);
        W_ZF=Hh_H_inv*H';

        %% Applying ZF
        y_ZF = W_ZF*y;
        H_eff=W_ZF*H;

		%% Translating to Q15 format
		y_ZF_fxp_data = int16(floor(y_ZF*pow2(15)));
		H_eff_fxp = int16((floor(H_eff*simparms.cw.scale_coef*pow2(15)))); % Perfect Chest
        

		%% Insert dummy pilots
		y_ZF_fxp(1:simparms.NB_ANTENNAS_RX,data_idx) = y_ZF_fxp_data;

        %% Reorder for processing
        H_eff_t = repmat([real(H_eff_fxp(:)) imag(H_eff_fxp(:))].',simparms.nb_re_per_frame,1);

        for ii=1:(nr)
            y_ZF_fxp_t(:,ii) = reshape([real(y_ZF_fxp(ii,:)); imag(y_ZF_fxp(ii,:))],2*simparms.nb_re_per_frame,1);
        end

 		%% Compute Scaling
    	avg = DlschChannelLevel(H_eff_t,simparms);
        [max_av_ch, index_max]=max(avg(:));

        simparms.log2_maxh = max(double(Log2Approx(max_av_ch))-10,0);

        %% Inner receiver loop
  		llrp0 = 1; % LLR pointer
        llrp1 = 1; % LLR pointer
        llrp2 = 1; % LLR pointer
        llrp3 = 1; % LLR pointer
        for symbol = 4:14
  			idxs = 2*(symbol-1)*simparms.nb_re_per_symbol + 1;
 			idxe = 2*(symbol-1)*simparms.nb_re_per_symbol + 2*simparms.nb_re_per_symbol;
            
   			[ymf0(idxs:idxe,:),...
 			 Heff0(idxs:idxe,:)...
 			 H_eff_mag0(idxs:idxe,:)...
 			 H_eff_magb0(idxs:idxe,:)]...
             = DlschChannelCompensation(y_ZF_fxp_t,H_eff_t,simparms,simparms.cw,symbol-1);


            %% LLR computation

            %Layer 0
             llr0 = DlschSISOLLR([ymf0(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 [H_eff_mag0(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 [H_eff_magb0(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 simparms,...
                 symbol-1);


 			llr_L0(llrp0:llrp0+length(llr0)-1,:) = llr0;
            llrp0 = llrp0 + length(llr0);

            %Layer 1
            llr1 = DlschSISOLLR([ymf0(idxs:idxe,6); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 [H_eff_mag0(idxs:idxe,6); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 [H_eff_magb0(idxs:idxe,6); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 simparms,...
                 symbol-1);

 			llr_L1(llrp1:llrp1+length(llr1)-1,:) = llr1;
            llrp1 = llrp1 + length(llr1);

            %Layer 2
            llr2 = DlschSISOLLR([ymf0(idxs:idxe,11); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 [H_eff_mag0(idxs:idxe,11); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 [H_eff_magb0(idxs:idxe,11); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 simparms,...
                 symbol-1);

 			llr_L2(llrp2:llrp2+length(llr2)-1,:) = llr2;
            llrp2 = llrp2 + length(llr2);

            %Layer 3
            llr3 = DlschSISOLLR([ymf0(idxs:idxe,16); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 [H_eff_mag0(idxs:idxe,16); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 [H_eff_magb0(idxs:idxe,16); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                 simparms,...
                 symbol-1);

 			llr_L3(llrp3:llrp3+length(llr3)-1,:) = llr3;
            llrp3 = llrp3 + length(llr3);

        end

        %% Layer Demapping
        
        llr_L0=-llr_L0;
        llr_L1=-llr_L1;
        llr_L2=-llr_L2;
        llr_L3=-llr_L3;
        
        LLR_demap=layer_demapping( llr_L0, llr_L1, llr_L2, llr_L3, simparms.cw.mod_order, simparms.num_layers, simparms.cw.G );
            
        %% A test if hard LLRs correspond to the encoded data

%         for ii=1:length(LLR_demap)
%             if LLR_demap(ii)>0
%                 LLR_reenc(ii)=1;
%             else
%                 LLR_reenc(ii)=0;
%             end
% 
%             if LLR_reenc(ii)~= edata0(ii)
%                 ii
%                msg = 'Reconstructed LLRs do not match encoded data. expecting a CRC failure';
%                error(msg)
%             end
%         end


%
		if (XFORMS)
            
          if (simparms.cw.mod_order>2) 
            ind=1;
            for k=1:12
              for i=1:(12*25/2) 
                I_0(i) = H_eff_mag0((2*25*12*k)+4*i,1)*cos(i*2*pi/(12*25/2));
                Q_0(i) = H_eff_mag0((2*25*12*k)+4*i+1,1)*sin(i*2*pi/(12*25/2));
                I_1(i) = H_eff_mag0((2*25*12*k)+4*i,6)*cos(i*2*pi/(12*25/2));
                Q_1(i) = H_eff_mag0((2*25*12*k)+4*i+1,6)*sin(i*2*pi/(12*25/2));
                I_2(i) = H_eff_mag0((2*25*12*k)+4*i,11)*cos(i*2*pi/(12*25/2));
                Q_2(i) = H_eff_mag0((2*25*12*k)+4*i+1,11)*sin(i*2*pi/(12*25/2));
                I_3(i) = H_eff_mag0((2*25*12*k)+4*i,16)*cos(i*2*pi/(12*25/2));
                Q_3(i) = H_eff_mag0((2*25*12*k)+4*i+1,16)*sin(i*2*pi/(12*25/2));
              ind=ind+1;
              end
            end
          end
          
          if (simparms.cw.mod_order>4) 
            ind=1;
            for k=1:12
              for i=1:(12*25/2) 
                Ib_0(i) = H_eff_magb0((2*25*12*k)+4*i,1)*cos(i*2*pi/(12*25/2));
                Qb_0(i) = H_eff_magb0((2*25*12*k)+4*i+1,1)*sin(i*2*pi/(12*25/2));
                Ib_1(i) = H_eff_magb0((2*25*12*k)+4*i,6)*cos(i*2*pi/(12*25/2));
                Qb_1(i) = H_eff_magb0((2*25*12*k)+4*i+1,6)*sin(i*2*pi/(12*25/2));
                Ib_2(i) = H_eff_magb0((2*25*12*k)+4*i,11)*cos(i*2*pi/(12*25/2));
                Qb_2(i) = H_eff_magb0((2*25*12*k)+4*i+1,11)*sin(i*2*pi/(12*25/2));
                Ib_3(i) = H_eff_magb0((2*25*12*k)+4*i,16)*cos(i*2*pi/(12*25/2));
                Qb_3(i) = H_eff_magb0((2*25*12*k)+4*i+1,16)*sin(i*2*pi/(12*25/2));
                
                Isum_0(i) = (H_eff_mag0((2*25*12*k)+4*i,1)+H_eff_magb0((2*25*12*k)+4*i,1))*cos(i*2*pi/(12*25/2));
                Qsum_0(i) = (H_eff_mag0((2*25*12*k)+4*i+1,1)+H_eff_magb0((2*25*12*k)+4*i+1,1))*sin(i*2*pi/(12*25/2));
                Isum_1(i) = (H_eff_mag0((2*25*12*k)+4*i,6)+H_eff_magb0((2*25*12*k)+4*i,6))*cos(i*2*pi/(12*25/2));
                Qsum_1(i) = (H_eff_mag0((2*25*12*k)+4*i+1,6)+H_eff_magb0((2*25*12*k)+4*i+1,6))*sin(i*2*pi/(12*25/2));
                Isum_2(i) = (H_eff_mag0((2*25*12*k)+4*i,11)+H_eff_magb0((2*25*12*k)+4*i,11))*cos(i*2*pi/(12*25/2));
                Qsum_2(i) = (H_eff_mag0((2*25*12*k)+4*i+1,11)+H_eff_magb0((2*25*12*k)+4*i+1,11))*sin(i*2*pi/(12*25/2));
                Isum_3(i) = (H_eff_mag0((2*25*12*k)+4*i,16)+H_eff_magb0((2*25*12*k)+4*i,16))*cos(i*2*pi/(12*25/2));
                Qsum_3(i) = (H_eff_mag0((2*25*12*k)+4*i+1,16)+H_eff_magb0((2*25*12*k)+4*i+1,16))*sin(i*2*pi/(12*25/2));
                
                
              ind=ind+1;
              end
            end
          end
            
            
			refreshdata(fig_llr0,'caller');
			drawnow;

			refreshdata(fig_ymf0,'caller');
			drawnow;
            
            if (simparms.cw.mod_order>2) 
                refreshdata(fig_IQ_0, 'caller');
                drawnow;
            end
            
            if (simparms.cw.mod_order>4) 
                refreshdata(fig_IQb_0, 'caller');
                drawnow;
                refreshdata(fig_IQsum_0, 'caller');
                drawnow;
                
            end
            
            refreshdata(fig_llr1,'caller');
			drawnow;
            
            refreshdata(fig_ymf1,'caller');
			drawnow;
            
            if (simparms.cw.mod_order>2) 
            refreshdata(fig_IQ_1, 'caller');
            drawnow;
            end
            
            if (simparms.cw.mod_order>4) 
                refreshdata(fig_IQb_1, 'caller');
                drawnow;
                refreshdata(fig_IQsum_1, 'caller');
                drawnow;
            end
            
            refreshdata(fig_llr2,'caller');
			drawnow;
            
            refreshdata(fig_ymf2,'caller');
			drawnow;
            
            if (simparms.cw.mod_order>2) 
              refreshdata(fig_IQ_2, 'caller');
              drawnow;
            end
            
            if (simparms.cw.mod_order>4) 
              refreshdata(fig_IQb_2, 'caller');
              drawnow;
              refreshdata(fig_IQsum_2, 'caller');
              drawnow;
            end
            
            refreshdata(fig_llr3,'caller');
			drawnow;
            
            refreshdata(fig_ymf3,'caller');
			drawnow;
            
            if (simparms.cw.mod_order>2) 
              refreshdata(fig_IQ_3, 'caller');
              drawnow;
            end
            
            if (simparms.cw.mod_order>4) 
              refreshdata(fig_IQb_3, 'caller');
              drawnow;
              refreshdata(fig_IQsum_3, 'caller');
              drawnow;
            end
            
            %pause(0.1);
            
		end
%
% 		%% Channel decoding
 		ret0 = DlschDecoding(LLR_demap,simparms,simparms.cw);
%
% 		% Check if decoded correctly
 		if (ret0 >= simparms.MAX_TURBO_ITERATIONS)
 			frame_errors(iSNR,1) =frame_errors(iSNR,1) + 1;
 		end

    end
    throughput(iSNR,1)=(1-frame_errors(iSNR,1)/N)*simparms.cw.TBS
	fprintf('********************SNR = %3.1f dB processed on %s********************\n',real(simparms.snr_db(iSNR)),datestr(now));
 	fprintf('Errors: %d/%d\nPe = %1.5f\n',frame_errors(iSNR,1),N, frame_errors(iSNR,1)/N);
    
 end
 figure(1)
 semilogy(SNRdB, frame_errors/N, 'r');
 grid on
 
 figure(2)
 plot(SNRdB, throughput/1000, 'b');
 grid on
    
 SNR_vs_BLER=[SNRdB.', frame_errors/N, throughput/1000];
% dlmwrite('ZF_mcs28.dat', SNR_vs_BLER, 'delimiter', '\t');

%% Post Processing
%  total_frame_erros = sum(simparms.frame_errors,2);
%  fer = total_frame_erros/N;
%  figure; semilogy(SNRdB,fer); grid;
%
%  disp(fer);
%
% profile viewer
toc

