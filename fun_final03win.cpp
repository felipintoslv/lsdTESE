#include "fun_head.h"

//**************************/
//PARAMETERS
//**************************/
/*
 MARKET
  AvKnowhow* - Represents: average know-how on technology * weighted by market share (ms*Sucrose)
               Used by: TotalProduction
               Initialization: 0
  CostFixMin/Max - Represents: minimum/maximum fix cost per mill
                   Used by: Decide
                   Initialization: >0
  CostVarAgricMin/Max - Represents: minimum/maximum variable cost per kiloton of sugar-cane harvested
                        Used by: Decide
                        Initialization: >0
  KnowhowLeak - Represents: Control the rate at which private know-how leaks to public know-how
                Used by: KnowhowInit0
                Initialization: =0, no leakage
                                >0, leakage
  LandPrice - Represents: price of land as a proxy for extra costs when making greenfield investment
              Used by: ComputeValueNewMill, Decide
              Initialization: >0
              ***PS: should be upgraded and become a variable
  LearningFactor - Represents: speed of learning to use a technology
                   Used by: ProductionKg, as maximum for a uniform draw
                   Initialization: =1, can at one step learn perfect usage
                                   <1, maximum of yet to learn that can be learned at one step
  MinProdMult - Represents: minimum accepted productivity for a capital good to be used as a proportion of the average productivity of capital goods on supply
                Used by: MinimumProductivity
                Initialization: <1
                ***PS: can be moved to Owner to define strategy.
  ms*SucroseX - Represents: market-share of total sucrose extraction of technology * in context X
               Used by: TotalProduction
               Initialization: 0
  PerceptionVar - Representes: perception mistakes
                  Used by: PerceivedProdKg
                  Initialization: =0, no mistake, perfect evaluation
                                  >0, perceived value is a draw from normal with average on true value and variance PerceptionVar
                  ***PS: can be moved to Owner or Mill
*  Price - Represents: price for selling final product
          Used by: CashFlow, ExpectedPrice
          Initialization: >0
          ***PS: should be upgraded and become a variable
  RenovationCosts - Represents: Costs associated with renovation of a unit of area
                    Used by: ComputeValueNewMill, ComputeValueSC, Decide
                    Initialization: >0
  SCGrowth - Represents: agricultural productivity advances
             Used by: SCPotential, as multiplier
             Initialization: =1, no growth
                             >1, exponential growth equal to 1-SCGrowth
  SCReduction - Represents: productivity loss over the harvests
                Used by: SCProduction, as multiplier
                Initialization: =1, no loss
                                <1, fall equal to 1-SCReduction
 OWNER
  CashOwner - Represents: Cash available to owner
              Used by: CashFlow, Decide
              Initialization: as desired
                              written by Decide
  ScrappingValue - Represents: minimum return on investment accepted, incorporate finance costs and market strategy
                   Used by: Decide
                   Initialization: >0
                                   low values, low financial costs and/or market share seeking agresive market strategy
                                   high values, high financial costs and/or profitability managing market strategy
 MILL
  CostFix - Represents: fix costs per mill
            Used by: CashFlow
            Initialization: >0
                            written by Decide for new instances
  CostVarAgric - Represents: variable cost per kiloton of sugar-cane harvested
                 Used by: CashFlow
                 Initialization: >0
                                 written by Decide for new instances
  Knowhow* - Represents: know-how accumulated by mill on operating capital goods of type *
             Used by: PerceivedProdKg, ProductionKg, ComputeValueKg
             Initialization: =1, perfect use
                             <1, some learning to be done
                             overwritten by ProductionKg, as learning takes place
  NewCapacity - Represents: capacity resulting of investments made in the iteration
                Used by: ComputeValueKg, Decide
                Initialization: 0
                                written by Decide, TotalProduction
  NewMill - Represents: boolean to indicate if Mill was created during simulation run
            Used by: -
            Initialization: 0
                            written by Decide
 SC
  Renovation - Represents: boolean if investment in renovation has been done in the period
               Used by: SCProduction, Decide, as boolean in both
               Initialization: 0
                               written by Decide, TotalProduction
 KG
  CapacityKg - Represents: capacity of the capital good
               Measure unit: kilotonnes of sugar-cane
               Used by: ProductionKg, SCExcess
               Initialization: according to type
                               written by Decide for new instances
  CostVarInd - Represents: variable industrial cost per ton of sucrose extracted
               Measure unit: Reais (R$) per ton of sugar equivalent sucrose
               Used by: Decide
               Initialization: >0
                               written by Decide for new instances
  ProductivityKg - Represents: productivity of the capital good
                   Used by: PerceivedProdKg, ProductionKg, SCExcess
                   Initialization: according to type
                                   written by Decide for new instances
  SCImput - Represents: sugar-cane assigned to the capital good as imput
            Used by: ProductionKg
            Initialization: 0
                            written by SCExcess
  TypeKg - Represents: index for type of capital good
           Used by: PerceivedProdKg, ProductionKg, as index
           Initialization: 0, press rolls
                           1, diffusion
                           3, ghost capital good, no production
                           written by Decide for new instances
 KSUPPLIER
  ProdGrowth - Represents: growth of new capital good productivity
               Used by: ProductivityKgS
               Initialization: =0, no growth in productivity
                               >0, growth
 TECHNOLOGY
  CapacityKgS - Represents: capacity of the capital good
                Measure unit: kilotonnes of sugar-cane
                Used by: Decide
                Initialization: ComputeValueNewMill, ComputeValueKg, Decide
  CostVarIndS - Represents: variable industrial cost per kilogram of sucrose extracted
                Measure unit: Reais (R$) per ton of sugar equivalent sucrose
                Used by: CashFlow
                Initialization: >0
                                written by Deide for new instances
  PriceKgS - Represents: price of capital good for sale
             Measure unit: Reais (R$) per ton of sugar equivalent sucrose
             Used by: ComputeValueNewMill, ComputeValueKg, Decide
             Initialization: according to type
  TypeKgS - Represents: index for type of capital good
            Used by: ComputeValueKg
            Initialization: 0, press rolls
                            1, diffusion
*/
//**************************/
//**************************/

MODELBEGIN

//**************************/
//MARKET
//**************************/
EQUATION("TotalProduction")
/*
Sum of the total production, zero NewCapacity and Renovation (for new iteration) and check market shares
*/

if (V("ActiveOwner") == 0)
  END_EQUATION(0);
  
  
 v[0]=SUM("ProductionOwner");
 v[1]=0;
 v[2]=0;
 v[4]=0; //market share of technology 0 in sucrose extracted
 v[5]=0; //market share of technology 1 in sucrose extracted
 v[8]=0; //average know-how for technology 0 weighted by market share in sucrose extracted
 v[9]=0; //average know-how for technology 1 weighted by market share in sucrose extracted
 
 CYCLE(cur1,"Owner")
 {
  CYCLES(cur1,cur2,"Mill")
  {
   v[6]=VS(cur2,"Knowhow0");
   v[7]=VS(cur2,"Knowhow1");
   WRITES(cur2,"NewCapacity",0);
   CYCLES(cur2,cur3,"SC")
   {
    WRITES(cur3,"Renovation",0);
   }
   CYCLES(cur2,cur3,"Kg")
   {
    v[3]=VS(cur3,"TypeKg");
    if(v[3]==0)
    {
     v[1]+=VS(cur3,"ProductionKg");
     v[8]+=VS(cur3,"ProductionKg")*v[6];
    }
    else if(v[3]==1)
    {
     v[2]+=VS(cur3,"ProductionKg");
     v[9]+=VS(cur3,"ProductionKg")*v[7];
    }
   }
  }
 }

 if((v[1]+v[2])>0) //avoid division by zero
 {
  v[4]=v[1]/(v[1]+v[2]); //market share of technology 0 in sucrose extracted
  v[5]=v[2]/(v[1]+v[2]); //market share of technology 1 in sucrose extracted
 }
 if(v[1]>0) //avoid division by zero
  v[8]=v[8]/v[1]; //average know-how for technology 0 weighted by market share in sucrose extracted
 if(v[2]>0) //avoid division by zero
  v[9]=v[9]/v[2]; //average know-how for technology 1 weighted by market share in sucrose extracted

 WRITE("ms0Sucrose",v[4]);
 WRITE("ms1Sucrose",v[5]);
 WRITE("AvKnowhow0",v[8]);
 WRITE("AvKnowhow1",v[9]);

RESULT(v[0])

EQUATION("AvKgProductivityS")
/*
Calculate the average productivity of capital goods on supply

*/

if (V("ActiveOwner") == 0)
  END_EQUATION(0);
  
v[0] = AVE("ProductivityKgS");

RESULT(v[0])


EQUATION("AvSCPotential")
/*
Calculate the average SCPotential
*/

if (V("ActiveOwner") == 0)
  END_EQUATION(0);
  
v[0]= AVE( "SCPotential" ); //average SCPotential

RESULT(v[0])

EQUATION("Price")
/*
- Historical average: 1.35 (equivalent to R$1,350/t)
- Standard deviation of the shock: 0.25
- Long-term trend: 1.5% p.a.
- Sinusoidal cycle with a 6-year period and amplitude of ±0.2
*/

v[0] = VL("Price", 1); // previous price
v[1] = 0.25; // standard deviation of the shock (0.25)
v[2] = 0.6 * VL("Shock", 1) + 0.4 * norm(0, v[1]); // smoothed shock
WRITE("Shock", v[2]); // shock memory

v[3] = 0.015 * v[0]; // real trend of 1.5% p.a.
v[4] = 1.35; // equilibrium price (R$1.35)
v[5] = 0.25; // mean-reversion force

v[6] = 0.2; // cycle amplitude (±0.2 = ±R$200)
v[7] = 6.0; // cycle period (6 years)
v[8] = V("t");

v[9] = v[6] * sin(2 * M_PI * v[8] / v[7]); // oscillatory cycle

v[10] = v[0] + v[2] + v[3] + v[5] * (v[4] - v[0]) + v[9];

// segurança contra valores negativos
if (v[10] <= 0.0)
  v[10] = 0.01;  // valor mínimo simbólico

RESULT(v[10])


/*
EQUATION("Price")
/*
Price per ton of sugar
*/
/*
v[0]=1.1*VL("Price",20);

RESULT(v[0])
*/

EQUATION("PubKnowhow0")
/*
Public know-how for technology 0
Represents the spilling of private know-how into public know-how
*/

// Ensure TotalProduction is calculated before proceeding
V("TotalProduction"); 

// v[1]: Public know-how in the previous period
v[1] = VL("PubKnowhow0", 1); 

// v[2]: Average know-how
v[2] = V("AvKnowhow0"); 

// v[3]: Initial difference between average and previous public know-how
v[3] = v[2] - v[1];

// v[4]: Market share of technology 0
v[4] = V("ms0Sucrose");

// v[5]: Knowhow leakage rate
v[5] = V("KnowhowLeak");

// v[6]: New difference considering leakage, incorporating market share influence
v[6] = v[3] * (1 - v[5] * v[4] * v[4]); 

// v[0]: Updated public know-how, adjusting for the leakage
v[0] = v[2] - v[6]; 

RESULT(v[0])

EQUATION("PubKnowhow1")
/*
Public know-how for technology 1
Represents the spilling of private know-how into public know-how
*/

// Ensure TotalProduction is calculated before proceeding
V("TotalProduction"); 

// v[1]: Public know-how in the previous period
v[1] = VL("PubKnowhow1", 1); 

// v[2]: Average know-how
v[2] = V("AvKnowhow1"); 

// v[3]: Initial difference between average and previous public know-how
v[3] = v[2] - v[1];

// v[4]: Market share of technology 1
v[4] = V("ms1Sucrose");

// v[5]: Knowhow leakage rate
v[5] = V("KnowhowLeak");

// v[6]: New difference considering leakage, incorporating market share influence
v[6] = v[3] * (1 - v[5] * v[4] * v[4]); 

// v[0]: Updated public know-how, adjusting for the leakage
v[0] = v[2] - v[6]; 

RESULT(v[0])

EQUATION("ComputeValueNewMill")
/*
Compute the scrapping value of building a new mill for the owner that called it
*/

if (V("ActiveOwner") == 0)
  END_EQUATION(0);
  
  
v[0]=0;
v[1]=VS(c,"ExpectedPrice");
v[2]=0;//V("KnowhowInit");
v[3]=V("LandPrice");
v[4]=V("RenovationCosts");
v[14]=UNIFORM(V("CostVarAgricMin"),V("CostVarAgricMax"));//v[14]=VS(c,"CostVarAgric");
v[17]=0;//VS(c,"CostFix");
v[18]=V("AvSCPotential");

v[5]=0; //productivity of best capital good on supply
v[6]=0; //price of best capital good on supply
v[7]=0; //capacity of best capital good on supply
v[11]=0; //best (simple) scrapping value

// Itera sobre todos os fornecedores de tecnologia
CYCLE(cur4,"KSupplier")
{
    CYCLES(cur4,cur5,"Technology")
    {
        v[8] = VS(cur5, "ProductivityKgS");
        v[9] = VS(cur5, "PriceKgS");
        v[15] = VS(cur5, "CostVarIndS") / 1000;
        v[10] = (v[1] - v[15]) * v[8] / v[9]; // simple scrapping value of capital good on supply, only for ranking
        if (v[10] > v[11]) // if this is the best option available
        {
            v[11] = v[10]; // scrapping value of best capital good on supply
            v[5] = v[8]; // productivity of best capital good on supply
            v[6] = v[9]; // price of best capital good on supply
            v[7] = VS(cur5, "CapacityKgS"); // capacity of best capital good on supply
            v[16] = VS(cur5, "CostVarIndS") / 1000; // variable cost of production with best capital good on supply
            if (VS(cur5, "TypeKgS") == 0) // public know-how to use
                v[2] = VL("PubKnowhow0", 1);
            else if (VS(cur5, "TypeKgS") == 1)
                v[2] = VL("PubKnowhow1", 1);
        }
    }
}

if (v[11] == 0) END_EQUATION(0);

v[12] = (v[5] * (1 - v[2]) * (v[1] - v[16]) - v[14]) / (v[3] + v[4] + v[6] / v[7]);
v[13] = v[17] / (v[18] * (v[3] + v[4] + v[6] / v[7]));

v[0] = v[12] - v[13]; // scrapping value of new mill

RESULT(v[0])



EQUATION("ComputeValueKg")
/*
Compute the scrapping value of buying new capital good for the mill that called it.
Should always be called by a Mill
*/

if (V("ActiveOwner") == 0)
  END_EQUATION(0);
  
v[0] = 0;             // v[0]: Best scrapping value found so far
v[1] = VS(c, "ExpectedPrice");     // v[1]: Expected price from the calling mill
v[2] = 0;             // v[2]: Public know-how (initialized to 0, will be updated later)
v[3] = V("LandPrice");            // v[3]: Land price
v[4] = VS(c, "SCExcess") - VS(c, "NewCapacity"); // v[4]: Available sugar cane 
v[11] = VS(c, "CostVarAgric");    // v[11]: Variable cost of agriculture
v[12] = 0;            // v[12]: VS(c, "CostFix"); // Fixed cost (currently 0)
v[18] = V("AvSCPotential");       // v[18]: Average sugar cane potential

// Iterate over all technology suppliers
CYCLE(cur4, "KSupplier")
{
    // Iterate over technologies offered by the supplier
    CYCLES(cur4, cur5, "Technology")
    {
        v[5] = VS(cur5, "TypeKgS");  // v[5]: Technology type
        v[6] = 0;                         // v[6]: Mill's know-how for this technology

        if (v[5] == 0)
            v[6] = VS(c, "Knowhow0");
        else if (v[5] == 1)
            v[6] = VS(c, "Knowhow1");

        v[7] = VS(cur5, "CapacityKgS");      // v[7]: Technology capacity
        v[8] = VS(cur5, "PriceKgS");        // v[8]: Technology price
        v[9] = VS(cur5, "ProductivityKgS");  // v[9]: Technology productivity
        v[13] = VS(cur5, "CostVarIndS") / 1000; // v[13]: Technology variable industrial cost

        // Calculate scrapping value for this technology
        // (See detailed explanation below)
        v[10] = ((v[1] - v[11]/v[9] - v[13]) * min(v[7], v[4]) * (1 - v[6]) * v[9] - v[12]) / v[8];
        
        // Apply lock-in penalty to the most efficient technology (type 1)
		if (v[5] == 1) // type 1 = diffusion
		{
    		v[10] *= (1 - V("LockInBias")); // artificially reduce its value
		}


        if (v[10] > v[0]) 
        {
            // Update best scrapping value and associated technology
            v[0] = v[10]; 
            c->hook = cur5; 

            // Update public know-how based on technology type
            if (v[5] == 0) {
                v[2] = VL("PubKnowhow0", 1);
            } else if (v[5] == 1) {
                v[2] = VL("PubKnowhow1", 1);
            }
        }
    }
}

RESULT(v[0])


EQUATION("ComputeValueSC")
/*
Compute the scrapping value of renovating the sugar cane area that called it. Should always be called by a SC
*/

if (V("ActiveOwner") == 0)
  END_EQUATION(0);
  
v[1] = VS(c, "ExpectedPrice");           // v[1]: Expected price from the calling sugar cane area
v[2] = VS(c, "AvKgProductivity");   // v[2]: Average productivity of capital goods in the mill
v[3] = V("RenovationCosts");           // v[3]: Renovation costs
v[4] = VS(c, "CostVarAgric");           // v[4]: Variable cost of agriculture
v[5] = VS(c, "AvCostVarInd") / 1000;   // v[5]: Average variable industrial cost (per ton)
v[6] = 0; //VS(c, "CostFix");        // v[6]: Fixed cost (currently 0)
v[7] = VS(c, "SCPotential");             // v[7]: Sugar cane potential
v[8] = VS(c, "SCProduction");           // v[8]: Current sugar cane production

// Calculate the increase in sugar cane production from renovation
v[9] = v[7] - v[8]; // v[9]: delta SC from renovation

// If average productivity or potential increase is zero, no benefit from renovation, so return 0
if (v[2] == 0 || v[9] == 0) {
    END_EQUATION(0);
}

// Calculate the scrapping value of renovation
// (Profit per unit production) * (Productivity increase) / (Renovation costs) - (Fixed cost) / (Production increase * Renovation costs)
v[0] = ((v[1] - v[4] / v[2] - v[5]) * v[9] / v[3]) - v[6] / (v[9] * v[3]);

RESULT(v[0]);


EQUATION("LockInBias")

/*
Lock-in bias with exponential decay composed of time and the market share of the efficient technology.

The function decays faster the greater the market share of the efficient technology,
but always decreases over time (simulating institutional learning).

(Not used)
*/

v[0] = V("Biasmax"); // initial bias (e.g.: 0.25)
v[1] = V("Biasmin"); // minimum bias (e.g.: 0.05)
v[2] = V("BiasDecay"); // base decay rate (e.g.: 0.05)
v[3] = V("ms1Sucrose"); // market share of the efficient technology

double t_ = t; // simulation time

// Decay composition: time and market share
double decay_rate = v[2] * (1 + 10 * v[3]); // 5 is the sensitivity to market share

v[4] = v[0] * exp(-decay_rate * t_); // exponential decay

// Prevents the bias from falling below the defined minimum
if (v[4] < v[1])
    v[4] = v[1];

RESULT(v[4])


//**************************/
//**************************/

//**************************/
//OWNER
//**************************/

EQUATION("ActiveOwner")
/*
Indicates whether the owner is active in the simulation (1 = active, 0 = inactive).
Updated only by the CashFlow equation.
*/
RESULT(VL("ActiveOwner", 1))


EQUATION("TypeOwner")
/*
Type of Owner, according to technologies used. Return 0 if only use technology 0, 1 if only use technology 1, 2 if uses both and 4 otherwise
*/

if (V("ActiveOwner") == 0)
  END_EQUATION(0);


 v[0]=4;

 CYCLE(cur,"Mill")
 {
  if(v[0]!=2)
  {
   v[1]=VS(cur,"TypeMill");
   if(v[0]==4)
    v[0]=v[1];
   else if(v[0]!=v[1])
    v[0]=2;
  }
 }

RESULT(v[0])

EQUATION("NumberMills")
/*
Number of Mills under Owner and calculate market-shares within Owner
*/

if (V("ActiveOwner") == 0)
  END_EQUATION(0);


 V("ProductionOwner");
 v[0]=0;
 
 CYCLE(cur,"Mill")
 {
  v[0]++;
 }
 
 v[1]=0;
 v[2]=0;
 v[3]=0;
 v[4]=0;
 v[5]=0;
 v[6]=0;
 v[7]=0;
 v[8]=0;
 v[9]=0;
 
 CYCLE(cur1,"Mill")
 {
  v[6]=VS(cur1,"Knowhow0");
  v[7]=VS(cur1,"Knowhow1");
  CYCLES(cur1,cur2,"Kg")
  {
   v[3]=VS(cur2,"TypeKg");
   if(v[3]==0)
   {
    v[1]+=VS(cur2,"ProductionKg");
    v[8]+=VS(cur2,"ProductionKg")*v[6];
   }
   else if(v[3]==1)
   {
    v[2]+=VS(cur2,"ProductionKg");
    v[9]+=VS(cur2,"ProductionKg")*v[7];
   }
  }
 }
  
 if((v[1]+v[2])>0) //avoid division by zero
 {
  v[4]=v[1]/(v[1]+v[2]); //market share of technology 0 in sucrose extracted
  v[5]=v[2]/(v[1]+v[2]); //market share of technology 1 in sucrose extracted
 }
 if(v[1]>0) //avoid division by zero
  v[8]=v[8]/v[1]; //average know-how for technology 0 weighted by market share in sucrose extracted
 if(v[2]>0) //avoid division by zero
  v[9]=v[9]/v[2]; //average know-how for technology 1 weighted by market share in sucrose extracted

 WRITE("ms0SucroseOwner",v[4]);
 WRITE("ms1SucroseOwner",v[5]);
 WRITE("AvKnowhow0Owner",v[8]);
 WRITE("AvKnowhow1Owner",v[9]);

RESULT(v[0])

EQUATION("SCOwner")
/*
Sum the total production of sugar-cane under one owner
*/

if (V("ActiveOwner") == 0)
  END_EQUATION(0);


 v[0]=SUM("SCMill");

RESULT(v[0])

EQUATION("ProductionOwner")
/*
Sum of the production under the owner
*/

if (V("ActiveOwner") == 0)
  END_EQUATION(0);


 v[0]=SUM("ProductionMill");

RESULT(v[0])




EQUATION("WasteOwner0")
// Comment

if (V("ActiveOwner") == 0)
  END_EQUATION(0);

v[0] = SUM("WasteMill0");

RESULT( v[0] )


EQUATION("WasteOwner1")
// Comment

if (V("ActiveOwner") == 0)
  END_EQUATION(0);


v[0] = SUM("WasteMill1");

RESULT( v[0] )

EQUATION("CashFlow")
/*
Manage cash flow
Production is sold, costs are paid and all revenue is centralized at the owner and investments are decided
Returns total income minus cost under the owner
*/

v[1]=V("ProductionOwner");
v[2]=V("Price");
v[3]=V("CashOwner");
v[6]=V("WasteOwner0");
v[7]=V("WasteOwner1");
v[9]=0; //total fixed cost under the owner
v[12]=0; //variable agricultural cost
v[13]=0; //variable industrial cost
v[21]=0; //total revenue from differentiated bagasse

CYCLE(cur1,"Mill")
{
  v[9]+=VS(cur1,"CostFix"); //sum fixed costs
  v[12]+=VS(cur1,"CostVarAgric")*VS(cur1,"SCMill");

  CYCLES(cur1, cur2,"Kg")
  {
    v[13]+=VS(cur2,"CostVarInd")*VS(cur2,"ProductionKg")/1000;

    // Bagasse revenue per technology (same price, but costs differ)
    v[30] = VS(cur2,"Wastekg0");
    v[31] = VS(cur2,"Wastekg1");
    v[32] = V("PriceBagasseKg");
    v[21] += (v[30] + v[31]) * v[32];
  }
}

v[0]=(v[1]*v[2]) + v[21] - v[9] - v[12] - v[13]; //cash flow
v[4]=v[3]+v[0]; //new cash

WRITE("CashOwner",v[4]);

if (v[4] < 0)
  WRITE("ActiveOwner", 0);  // desativa o Owner



do {
  v[5]=V("Decide");
  if((v[5]+4)<0)
    END_EQUATION(-99999);
} while(v[5]>0);

RESULT(v[0])

EQUATION("ExpectedPrice")
/*
Price the owner expects to receive for production
*/

 v[0]=V("Price");

RESULT(v[0])

EQUATION("Decide")
/*
Select the best investment option available and implement it.
Return the kind of option chosen:
1 for sugar cane renovation,
2 for investment in existing mill (buy capital good),
3 for creating a new mill and
-1 when no available options are good enough (scrapping value below minimum limit) or not affordable for the owner
*/

if (V("ActiveOwner") == 0)
    END_EQUATION(-99); // inactive owner does not invest

// Ensures that production was updated before the decision
V("TotalProduction");

// ? Forces the execution of SCAge for all sugar cane areas
CYCLE(cur1, "Mill") {
    CYCLES(cur1, cur2, "SC") {
        V_CHEAT("SCAge", cur2);
    }
}

v[0]=0; // investment option (default: 0 = no investment)
v[1]=0; // best scrapping value
v[2]=V("ScrappingValue");

// Tests new mill creation
v[3]=V("ComputeValueNewMill");
if(v[3]>v[1]) {
    v[1]=v[3];
    v[0]=3;
}

// Tests capital goods purchase for existing mills
CYCLE(cur1,"Mill") {
    v[3]=V_CHEAT("ComputeValueKg",cur1);
    if(v[3]>v[1]) {
        v[1]=v[3];
        v[0]=2;
        cur3=cur1; // Mill to invest in
        cur4=cur1->hook; // Selected technology
    }

    // Tests agricultural area renovation
    CYCLES(cur1,cur2,"SC") {
        v[3]=V_CHEAT("ComputeValueSC",cur2);
        v[4]=VS(cur2,"Renovation");
        v[5]=VS(cur2,"SCAge");
        v[6]=V("MinAgeSCForRenovation");
        if(v[3]>v[1] && v[4]==0 && v[5]>=v[6]) {
            v[1]=v[3];
            v[0]=1;
            cur3=cur2; // SC to be renovated
        }
    }
}

if(v[1]<v[2])
    END_EQUATION(-4);

// Checks resources and executes investment
v[5]=V("RenovationCosts");
v[6]=V("ExpectedPrice");
v[7]=V("LandPrice");
v[8]=V("AvSCPotential");
v[9]=V("CashOwner");

switch(int(v[0])) {
    case 1: // Renovation
        v[10]=VS(cur3,"SCPotential");
        v[11]=VS(cur3,"SCProduction");
        v[12]=v[5]*(v[10]-v[11]);
        v[13]=v[9]-v[12];
        if(v[13]<0)
            END_EQUATION(-1);
        WRITE("CashOwner",v[13]);
        WRITES(cur3,"Renovation",1);
        WRITES(cur3,"ResetSCAge",1); // <- new line
        break;

    case 2: // New capital good
        v[20] = VS(cur4, "CapacityKgS");
        v[21] = VS(cur3, "TotalKgCapacity");
        v[22] = V("MaxKgCapacityPerMill");
        if ((v[21] + v[20]) > v[22])
            END_EQUATION(-11);
        v[10] = VS(cur4, "PriceKgS");
        v[11] = v[9] - v[10];
        if (v[11] < 0)
            END_EQUATION(-2);
        WRITE("CashOwner", v[11]);
        cur5 = ADDOBJS(cur3, "Kg");
        WRITES(cur5, "CapacityKg", v[20]);
        WRITES(cur5, "ProductivityKg", VS(cur4, "ProductivityKgS"));
        WRITES(cur5, "TypeKg", VS(cur4, "TypeKgS"));
        WRITES(cur5, "SCImput", 0);
        WRITES(cur5, "CostVarInd", VS(cur4, "CostVarIndS"));
        WRITES(cur3, "NewCapacity", VS(cur3, "NewCapacity") + v[20]);
        break;

    case 3: // New mill
        if (COUNTS(c, "Mill") >= V("MaxMills"))
            END_EQUATION(-88);
        v[10] = (v[5] + v[7]) * v[8] + V("InfraCost");
        v[11] = v[9] - v[10];
        if (v[11] < 0)
            END_EQUATION(-3);
        WRITE("CashOwner", v[11]);
        cur3 = ADDOBJ("Mill");
        WRITES(cur3, "NewCapacity", 0);
        WRITES(cur3, "NewMill", 1);
        v[12] = max(0.1, V("PubKnowhow0") - 0.2);
        v[13] = max(0.1, V("PubKnowhow1") - 0.2);
        WRITES(cur3, "Knowhow0", v[12]);
        WRITES(cur3, "Knowhow1", v[13]);
        v[14] = UNIFORM(V("CostVarAgricMin"), V("CostVarAgricMax"));
        WRITES(cur3, "CostVarAgric", v[14]);
        v[15] = UNIFORM(V("CostFixMin"), V("CostFixMax"));
        WRITES(cur3, "CostFix", v[15]);
        CYCLES(cur3, cur4, "SC") {
            WRITES(cur4, "Renovation", 1);
            WRITES(cur4, "SCProduction", 0);
            WRITES(cur4, "SCPotential", v[8]);
        }
        CYCLES(cur3, cur5, "Kg") {
            WRITES(cur5, "CapacityKg", 0);
            WRITES(cur5, "ProductivityKg", 0);
            WRITES(cur5, "SCImput", 0);
            WRITES(cur5, "TypeKg", 3);
        }
        break;

    default:
        END_EQUATION(-10);
}
RESULT(v[0])

//**************************/
//**************************/

//**************************/
//MILL
//**************************/


EQUATION("Occurrences_of_Poisson" )
/*
produce occurrences of energy chocks in the mill
*/

v[0] = V( "num_occu" ); // mean of occurrences in the  year
v[1] = poisson( v[0] );
v[2] = round ( v[1] );

RESULT( v[2] )


EQUATION("TypeMill")
/*
Type of Mill, according to technologies used. Return 0 if only use technology 0, 1 if only use technology 1, 2 if uses both and 4 otherwise
*/

 v[0]=4;

 CYCLE(cur,"Kg")
 {
  if(v[0]!=2)
  {
   v[1]=VS(cur,"TypeKg");
   if(v[0]==4)
    v[0]=v[1];
   else if(v[0]!=v[1])
    v[0]=2;
  }
 }

RESULT(v[0])

EQUATION("SCMill")
/*
Sum of all sugar-cane production under the mill
*/

if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);


 v[0]=SUM("SCProduction");

RESULT(v[0])

EQUATION("MinimumProductivity")
/*
Minimum accepted productivity for a capital good to be used
*/

 v[1]=V("MinProdMult");
 v[2]=V("AvKgProductivityS");
 
 v[0]=v[1]*v[2];

RESULT(v[0])

EQUATION("SCExcess")
/*
Amount of sugar-cane not processed
Is influenced by sugar-cane production and capacity of capital goods
First, it absorbs public know-how
Rank capital goods by productivity and supply them with sugar-cane imput (write SCImput). Control for disposal of old capital goods
*/
 
 if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);

 v[0]=V("SCMill");
 
 v[12]=V("Knowhow0");
 v[13]=VL("PubKnowhow0",1);
 if(v[13]>v[12]) //if public know-how is better than own know-how
  WRITE("Knowhow0",v[13]); //absorb public know-how
 v[14]=V("Knowhow1");
 v[15]=VL("PubKnowhow1",1);
 if(v[15]>v[14]) //if public know-how is better than own know-how
  WRITE("Knowhow1",v[15]); //absorb public know-how

 SORT("Kg","PerceivedProdKg","DOWN"); //rank capital goods of the mill by their productivity
 /*
 cur = SEARCH("Kg");
 
 v[1]=VS(cur,"CapacityKg");
 v[2]=max(0,min(v[0],v[1])); //if there is no more sugar-cane, becomes 0. min(sugar-cane,capacity) otherwise
 v[3]=VS(cur,"ProductivityKg");
 v[4]=V("MinimumProductivity");
 if(v[3]<v[4])  //if capital good productivity is too low
 {
   v[2]=0;      //assign it no imputs
  } 
 WRITES(cur,"SCImput",v[2]); //assign the remaining sugar-cane to the capital good up to its capacity
 v[0]-=v[2]; //reduce remaining sugar-cane by amount assigned to capital good

*/
 CYCLE(cur,"Kg") //go through all capital goods in the mill
 {
  v[1]=VS(cur,"CapacityKg");
  v[2]=max(0,min(v[0],v[1])); //if there is no more sugar-cane, becomes 0. min(sugar-cane,capacity) otherwise
  v[3]=VS(cur,"ProductivityKg");
  v[4]=V("MinimumProductivity");
  if(v[3]<v[4]) //if capital good productivity is too low
   v[2]=0; //assign it no imputs
  WRITES(cur,"SCImput",v[2]); //assign the remaining sugar-cane to the capital good up to its capacity
  v[0]-=v[2]; //reduce remaining sugar-cane by amount assigned to capital good
 }
 
RESULT(v[0])


EQUATION("ProductionMill")
/*
Summ of all capital goods productions and losses under the mill
*/

if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);

V("SCExcess"); //execute SCExcess first to update values of SCImput

//v[1] = V( "TotalProductionMill" );
v[2] = 0;// V( "ProductionLossesMill");//"ProductionKgLosses" );

v[0] = SUM("ProductionKg");
/*
 v[0]=0; 
 CYCLE(cur,"Kg")
 {
  v[0]+=VS(cur,"ProductionKg");
 }
*/
RESULT( v[0] )

EQUATION("WasteMill0")
// Comment

if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);

v[0] = SUM("Wastekg0");

RESULT( v[0] )

EQUATION("WasteMill1")
// Comment

if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);

v[0] = SUM("Wastekg1");

RESULT( v[0] )

EQUATION("AvKgProductivity")
/*
Average productivity of all capital good under the mill
Is influenced by the productivity of capital goods
*/

 if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);

 v[1]=V("SCMill");
 v[2]=0;//V("SCExcess");
 v[3]=V("ProductionMill");
 
 v[4]=v[1]-v[2];
 if(v[4]==0)
  END_EQUATION(0);
 
 v[0]=v[3]/v[4];

RESULT(v[0])

EQUATION("AvCostVarInd")
/*
Average variable cost of all capital good under the mill
*/
/*
 v[1]=0;
 v[2]=0;
 
 CYCLE(cur,"Kg")
 {
  v[1]+=VS(cur,"CostVarInd");
  v[2]+=1;
 }
 
 if(v[2]==0)
  END_EQUATION(0);
 
 v[0]=v[1]/v[2];

*/
 
 if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);


v[0] = AVE( "CostVarInd" );


RESULT(v[0])


EQUATION("TotalKgCapacity")

/*
Sum of the installed capacities of all capital goods (Kg) within this mill.
Used to limit new investments per mill.
*/

if (VS(p, "ActiveOwner") == 0)
    END_EQUATION(0);

v[0] = SUMS(p, "CapacityKg"); // 'p' references the parent object (Mill)

RESULT(v[0])



//**************************/
//**************************/

//**************************/
//SC
//**************************/

EQUATION("SCPotential")
/*
Potential production of sugar-cane of the mill
Is influenced by the area and by agricultural productivity
Grows exponetilly over time, representing agricultural productivity advances
Measure unit: kilotonnes of sugar-cane
*/

if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);

 v[1]=VL("SCPotential",1);
 v[2]=V("SCGrowth");
 
 v[0]=v[1]*v[2]; //potential grows over time

RESULT(v[0])

EQUATION("SCProduction")
/*
Actual production of sugar-cane of the mill
Is influenced by area, agricultural productivity, sugar-cane productivity loss over the harvests and by investiments in renovation
Falls over time. When renovation is made, increases to equal SCPotential
*/

if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);

 v[1]=VL("SCProduction",1);
 v[2]=V("SCReduction");
 v[3]=V("Renovation");
 v[4]=V("SCPotential");
 
 v[0]=v[1]*v[2]; //production falls over time
 if(v[3]) //if renovation is made
  v[0]=v[4]; //production raises to potential

RESULT(v[0])

EQUATION("SCAge")

 v[0] = VL("SCAge", 1);
 if (VL("ResetSCAge", 1) == 1) // ? uses value from previous time
     v[0] = 0;
 else
     v[0] += 1;

RESULT(v[0])
 

EQUATION("ResetSCAge")
/*
Auxiliary variable to reset the area's age only after confirmed renovation.
It will be written directly at the time of the investment decision.
*/
RESULT(0) // default value


//**************************/
//**************************/

//**************************/
//KG
//**************************/

EQUATION("PerceivedProdKg")
/*
Productivity of the capital good as perceived by the owner
Is influenced by the actual productivity of the capital good, the know-how in the mill and some mistake in perception
*/

if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);

 v[1]=V("TypeKg");
 v[2]=V("PerceptionVar");
 v[3]=V("ProductivityKg");
 v[4]=1000; //set know-how to absurd value
 if(v[1]==0) //set know-how to correct value
  v[4]=V("Knowhow0");
 else if (v[1]==1)
  v[4]=V("Knowhow1");
 else
  END_EQUATION(0);
 
 v[5]=v[3]*(1-v[4]); //effective productivity, taking into account know-how
 v[0]=norm(v[5],v[2]); //perceived productivity, taking into account actual productivity and perception error

RESULT(v[0])

EQUATION("ProductionKg")
// Sacarose extracted from sugar-cane
// Is influenced by productivity, know-how and imput
// After production, proportional to usage, increases know-how for its type

// V("SCExcess"); //execute SCExcess first to update values of SCImput

// Absorção do know-how público (revisar e descomentar se necessário)
/*
v[12]=V("Knowhow0");
v[13]=VL("PubKnowhow0",1);
if(v[13]>v[12]) //if public know-how is better than own know-how
  WRITES(p->up,"Knowhow0",v[13]); //absorb public know-how
v[14]=V("Knowhow1");
v[15]=VL("PubKnowhow1",1);
if(v[15]>v[14]) //if public know-how is better than own know-how
  WRITES(p->up,"Knowhow1",v[15]); //absorb public know-how
*/

if (VS(p, "ActiveOwner") == 0)
  END_EQUATION(0);

v[1]=V("TypeKg");
 v[2]=V("ProductivityKg");
 v[3]=1000; //set know-how to absurd value
 if(v[1]==0) //set know-how to correct value
  v[3]=V("Knowhow0");
 else if (v[1]==1)
  v[3]=V("Knowhow1");
 else
  END_EQUATION(0);
 v[4]=V("CapacityKg");
 v[5]=V("SCImput");
 v[6]=V("SCMill");
 v[7]=V("LearningFactor");
 
 v[0]=min(v[4],v[5])*v[2]*(1-v[3]); //production


 
if(v[1]==0) //set how many days stop the prodution
  v[12] = V( "stop_days_PressRoll" );
else if (v[1]==1)
  v[12]=V( "stop_days_Diff" );

v[13] = V( "Occurrences_of_Poisson" );

// Usando 365 dias por ano
v[14] = (v[12] / 365.0) * v[13] * v[0]; 

v[15] = v[0] - v[14];

v[8]=v[5]/v[6]; //normalize the amount produced 
v[9]=uniform(0,1); 
//v[10]=uniform(0,v[7]); //learning to be obtained 
v[11]=v[3]*(1-v[7]); //new know-how 
if(v[9]<v[8]) //if there is learning 
{
 if(v[1]==0) 
 WRITES(p->up,"Knowhow0",v[11]); //write new know-how else 
 if (v[1]==1) 
 WRITES(p->up,"Knowhow1",v[11]);
  } 

RESULT(v[15])


EQUATION("Wastekg0")
// Comment
 
 if (VS(p, "ActiveOwner") == 0)
   END_EQUATION(0);
 
 v[1]=V("TypeKg");
 v[2]=V("ProductionKg");
 if(v[1]==0) //set how many days stop the prodution
   v[0] = v[2]/4;
 
 RESULT( v[0] )
 
 EQUATION("Wastekg1")
 // Comment
 
 if (VS(p, "ActiveOwner") == 0)
   END_EQUATION(0);
 
 v[1]=V("TypeKg");
 v[2]=V("ProductionKg");
 if(v[1]==1) //set how many days stop the prodution
   v[0] = v[2]/4;
 
RESULT( v[0] )


/*
EQUATION("ProductionKgLosses")
// Calcula as perdas de produção devido a paradas por choques de energia.
v[1] = V( "ProductionKg" );
v[2] = V( "TypeKg" );

if(v[2]==0) //set how many days stop the prodution
  v[3] = V( "stop_days_PressRoll" );
else if (v[2]==1)
  v[3]=V( "stop_days_Diff" );

v[4] = V( "Occurrences_of_Poisson" );

// Usando 365 dias por ano
v[0] = (v[3] / 365.0) * v[4] * v[1]; 

RESULT( v[0] )
*/
/// - (v[20] / 360) * v[40] * v[4]

//**************************/
//**************************/

//**************************/
//TECHNOLOGY
//**************************/

EQUATION("ProductivityKgSBase")
/*
Basic growth of productivity of capital good available to new investment
*/

  v[0]=0;
  v[1]=V("ProdGrowth");
  
  v[2]=norm(v[0],v[1]);
  if(v[2]>v[0])
   v[0]=v[2];
 
RESULT(v[0])


EQUATION("ProductivityKgS")
/*
Productivity of capital good available to new investment
Grows over time according to ProductivityKgSBase
*/

  v[1]=VL("ProductivityKgS",1);
  v[2]=V("ProductivityKgSBase");
  
  v[0]=v[1]+v[2];
 
RESULT(v[0])

//**************************/
//**************************/

//**************************/
//TEMP
//**************************/

//**************************/
//**************************/

MODELEND




void close_sim(void)
{

}


