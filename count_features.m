%function [fca, fcn, features]=count_features(data_id, labels)
function features=count_features(data_id, labels)
%	data=data_id(labels==1,:);
%	[un]=unique(data);
%	[nn,xx]=hist(data,un);
%	fca=int32([nn(:)(nn(:)!=0) xx]);
	data=data_id(labels==0,:);
%	[un]=unique(data);
%	[nn,xx]=hist(data,un);
%	fcn =int32([nn(:)(nn(:)!=0) xx]);
	[un]=unique(data_id);
	[nn,xx]=hist(data_id,un);
	features =int32([nn(:)(nn(:)!=0) xx zeros(numel(xx),1)]);
	for i=1:numel(features(:,2))
		tmp = sum(data(:)==features(i,2));
		features(i,3)=tmp;
#		features(i,4)=features(i,1)-tmp;
	end
end
