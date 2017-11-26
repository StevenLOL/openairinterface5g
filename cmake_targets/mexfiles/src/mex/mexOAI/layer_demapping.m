function [ LLR_demapped ] = layer_demapping( llr_l0, llr_l1, llr_l2, llr_l3, mod_order, n_layers, G )


        LLR_demapped = zeros(G, 1,'int16');
        
        if mod_order == 2 && n_layers ==4
        LLR_demapped(1:8:end) = llr_l0(1:2:end);
        LLR_demapped(2:8:end) = llr_l0(2:2:end);
        LLR_demapped(3:8:end) = llr_l1(1:2:end);
        LLR_demapped(4:8:end) = llr_l1(2:2:end);
        
        LLR_demapped(5:8:end) = llr_l2(1:2:end);
        LLR_demapped(6:8:end) = llr_l2(2:2:end);
        LLR_demapped(7:8:end) = llr_l3(1:2:end);
        LLR_demapped(8:8:end) = llr_l3(2:2:end);
        
        elseif mod_order == 4 && n_layers ==4
        LLR_demapped(1:16:end) = llr_l0(1:4:end);
        LLR_demapped(2:16:end) = llr_l0(2:4:end);
        LLR_demapped(3:16:end) = llr_l0(3:4:end);
        LLR_demapped(4:16:end) = llr_l0(4:4:end);
        
        LLR_demapped(5:16:end) = llr_l1(1:4:end);
        LLR_demapped(6:16:end) = llr_l1(2:4:end);
        LLR_demapped(7:16:end) = llr_l1(3:4:end);
        LLR_demapped(8:16:end) = llr_l1(4:4:end);
        
        
        LLR_demapped(9:16:end) =  llr_l2(1:4:end);
        LLR_demapped(10:16:end) = llr_l2(2:4:end);
        LLR_demapped(11:16:end) = llr_l2(3:4:end);
        LLR_demapped(12:16:end) = llr_l2(4:4:end);
        
        LLR_demapped(13:16:end) = llr_l3(1:4:end);
        LLR_demapped(14:16:end) = llr_l3(2:4:end);
        LLR_demapped(15:16:end) = llr_l3(3:4:end);
        LLR_demapped(16:16:end) = llr_l3(4:4:end);
        
        elseif mod_order == 6 && n_layers ==4
        LLR_demapped(1:24:end) = llr_l0(1:6:end);
        LLR_demapped(2:24:end) = llr_l0(2:6:end);
        LLR_demapped(3:24:end) = llr_l0(3:6:end);
        LLR_demapped(4:24:end) = llr_l0(4:6:end);
        LLR_demapped(5:24:end) = llr_l0(5:6:end);
        LLR_demapped(6:24:end) = llr_l0(6:6:end);
        
        LLR_demapped(7:24:end) = llr_l1(1:6:end);
        LLR_demapped(8:24:end) = llr_l1(2:6:end);
        LLR_demapped(9:24:end) = llr_l1(3:6:end);
        LLR_demapped(10:24:end) = llr_l1(4:6:end);
        LLR_demapped(11:24:end) = llr_l1(5:6:end);
        LLR_demapped(12:24:end) = llr_l1(6:6:end);
   
        
        LLR_demapped(13:24:end) = llr_l2(1:6:end);
        LLR_demapped(14:24:end) = llr_l2(2:6:end);
        LLR_demapped(15:24:end) = llr_l2(3:6:end);
        LLR_demapped(16:24:end) = llr_l2(4:6:end);
        LLR_demapped(17:24:end) = llr_l2(5:6:end);
        LLR_demapped(18:24:end) = llr_l2(6:6:end);
   
        
        LLR_demapped(19:24:end) = llr_l3(1:6:end);
        LLR_demapped(20:24:end) = llr_l3(2:6:end);
        LLR_demapped(21:24:end) = llr_l3(3:6:end);
        LLR_demapped(22:24:end) = llr_l3(4:6:end);
        LLR_demapped(23:24:end) = llr_l3(5:6:end);
        LLR_demapped(24:24:end) = llr_l3(6:6:end);
        
        else
            error('invalid code for layer demapping, add more cases for your parameters\n');
  
        end 

end

