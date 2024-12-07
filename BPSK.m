N=8192;
Ncp=round(N/10);
ConvN = 2*N;
ConvNcp = round(ConvN/10);
F=1000;
T=1/F;


% time and frequancy steps
f=((-N/2:1:(N)/2-1)*F).';
fs=((0:N-1)*F).';
t=((0:N-1)*T).';
tcp=((0:N+Ncp-1)*T).';


% creating tx signal
bits = double(rand(N,1)>0.5);
ConvBits = ConvCode1(bits);
S = 2*bits-1;
ConvS = 2*ConvBits-1;
Stx = ifft(fftshift(S))*sqrt(N);
ConvStx = ifft(fftshift(ConvS))*sqrt(ConvN);
Scp = Stx(N-Ncp+1:N);
ConvScp = ConvStx(ConvN-ConvNcp+1:ConvN);
NewS = cat(1,Scp,Stx);
ConvNewS = cat(1,ConvScp,ConvStx);



% noise
snr =[10:-0.5:-20];
N0 = 10.^(-snr./10);
noise = sqrt((N0)/2).*randn(N+Ncp,1)+1i*sqrt((N0)/2).*randn(N+Ncp,1);
ConvNoise = sqrt((N0)/2).*randn(ConvN+ConvNcp,1)+1i*sqrt((N0)/2).*randn(ConvN+ConvNcp,1);
NewS = NewS+noise;
ConvNewS = ConvNewS+ConvNoise;


% processing rx signal
Srx = NewS(size(Scp,1)+1:size(Stx,1)+size(Scp,1),:) ;
ConvSrx = ConvNewS(size(ConvScp,1)+1:size(ConvStx,1)+size(ConvScp,1),:) ;
S2 = fftshift(fft(Srx)/sqrt(N),1);
ConvS2 = fftshift(fft(ConvSrx)/sqrt(ConvN),1);
bitsRx = double(real(S2)>0);
ConvBitsRx = double(real(ConvS2)>0);
ConvBrx = DecViterbi(ConvBitsRx,ConvBitsRx(1:ConvN,1),ConvBitsRx(1,1:size(snr,2)));


% compare rx and tx
cmp = bits==bitsRx(1:N,:);
BER = 1-mean(cmp,1);
ConvCmp = bits==ConvBrx(1:N,:);
ConvBER = 1-mean(ConvCmp,1);
%plots
% figure (1)
%  for i=1:size(snr,2)
%     clf;
%     subplot(2,1,1)
%     hold on
%     scatter(real(S2(:,i)),imag(S2(:,i)),'filled');
%     grid on
%     grid minor
%     xlabel('I');
%     ylabel('Q');
%     title('IQ with SNR',snr(i));
%     xlim([-2 2])
%     ylim([-2 2])
%     hold off
% 
% 
%     subplot(2,1,2)
%     hold on
%     semilogy(f,cmp(1:N,i),'o');
%     grid on
%     grid minor
%     xlabel('f,Гц');
%     ylabel('Bits');
%     title('Comparison');
%     hold off
%     pause(0.05)
%  end

 figure (2)
    semilogy(snr,BER,snr,ConvBER);
    grid on
    grid minor
    xlabel('SNR,db');
    ylabel('BER');
    legend('BPSK','Conv BPSK',Location='southwest')


