%% generate test signal data
% signal = randn(20000,1);
t0 = 0;
fs = 5000;
h = 1/fs;
tf = 100;
t = t0:h:tf;
signal = chirp(t,0,tf,2000);
fileID = fopen('test_sig_chirp_100.txt','w');
fprintf(fileID,'%f\n',signal);
fclose(fileID);