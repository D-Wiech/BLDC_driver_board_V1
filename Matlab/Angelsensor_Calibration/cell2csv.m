function cell2csv(filename,cellArray,delimiter)
% Writes cell array content into a *.csv file.
%
% CELL2CSV(filename,cellArray,delimiter)
%
% filename = Name of the file to save. [ i.e. 'text.csv' ]
% cellarray = Name of the Cell Array where the data is in
% delimiter = Seperating sign, normally:',' (it's default)
%
% by Sylvain Fiedler, KA, 2004
% modified by Rob Kohr, Rutgers, 2005 - changed to english and fixed delimiter
% modified by Dominik Geisler, Allegro MicroSystems, 2018 - removed 'eval' function
if nargin<3
delimiter = ',';
end
file = fopen(filename,'w');
for z=1:size(cellArray,1)
for s=1:size(cellArray,2)
var = cellArray{z,s};
if size(var,1) == 0
var = '';
end
if isnumeric(var) == 1
var = num2str(var);
end
fprintf(file,var);
if s ~= size(cellArray,2)
fprintf(file,[delimiter]);
end
end
fprintf(file,'\n');
end
fclose(file);