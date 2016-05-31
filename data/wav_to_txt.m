
filename = 'moonlight_sonata_movement_1';
[y,Fs] = audioread([filename '.wav']);
signal = y(1:round(end*0.05),1);
% 
fileID = fopen('moonlight5percent.txt','w');
fprintf(fileID,'%f\n',signal);
fclose(fileID);