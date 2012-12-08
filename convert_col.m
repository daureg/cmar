function [c] = convert_col(col, offset, binary)
	if (binary)
		c=int32(offset+col);
	else
		col = col + 1;
		ua=unique(col);
		rua=zeros(numel(ua),1);
		for i=1:numel(ua)
			rua(ua(i))=i;
		end
		c=int32(arrayfun(@(i) rua(i), col)+offset);
	end
end
