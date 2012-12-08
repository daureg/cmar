function D = changed_filled(a)
	max_unique=430;
	a(:,1)=convert_col(int16(a(:,1)), 0*max_unique, false);
	a(:,2)=convert_col(int16(a(:,2)), 1*max_unique, false);
	a(:,3)=convert_col(int16(40*a(:,3)), 2*max_unique, false);
	a(isnan(a(:,4))==1,4)=1;
	for i=5:size(a,2)
		a(:,i)=convert_col(a(:,i), (i-1)*max_unique, true);
	end
	D=int32(a);
end
