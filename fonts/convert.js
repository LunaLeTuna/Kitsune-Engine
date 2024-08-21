var max_line_length = 48;
var character_x_size = 59;
var character_y_size = 134;
var mll = max_line_length*character_x_size;
var i = 0;

max_line_length++
(`AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890.!?:;"\`',*&@#$()[]{}_-+=<>~/\| `
.split(''))
.map((a,index)=>{
		{
				if(index%max_line_length==max_line_length-1) i++;
				var line_place = i
		}
		return {
			"char":a,
			"pos":[index*character_x_size%mll,line_place*character_y_size],
			"size":[character_x_size,character_y_size]
		}
	}
)